# Transparent Clock in pure Tcl:

  array set Config {
    X,base   10
    Y,base   10
    X,incr  190
    Y,incr    0
  }

  proc drawTransDigit {rootname x y number} {
    set ret [list]
    if {[string is integer -strict $number] &&
      [string length $number] == 1
    } {
      set segmentList {
        a 02356789   10 0 40 10
        b 045689     0 10 10 40
        c 01234789   50 10 10 40
        d 2345689    10 50 40 10
        e 0268       0 60 10 40
        f 013456789  50 60 10 40
        g 0235689    10 100 40 10
      }
      foreach {segment group x1 y1 width height} $segmentList {
        if {[string first $number $group] != -1} {
          # Experiment with -bg, -highlightbackground and
          # -highlightthickness to get the look you like:
          lappend ret [toplevel $rootname$segment \
            -bg red -highlightbackground yellow \
            -highlightthickness 1]

          # You should also experiment with other stuff
          # which affects toplevels like -relief and
          # wm attribute -alpha etc.

          # Unfortunately, only windows support -topmost
          # which I consider the "proper" behavior
          if {[lindex [winfo server .] 0] == "Windows"} {
            wm attributes $rootname$segment -topmost 1
          }

          wm overrideredirect $rootname$segment 1
          incr x1 $x
          incr y1 $y
          wm geometry $rootname$segment ${width}x${height}+${x1}+${y1}
        }
      }
    }
    return $ret
  }

  proc drawTransNumber {rootname x y number} {
    set ret [list]
    foreach i [split $number {}] {
      set ret [concat $ret [drawTransDigit $rootname$x $x $y $i]]
      incr x 70
    }
    return $ret
  }

  # The code can indeed be simpler than this
  # but the simple version flickers too much
  # for my taste. All this voodoo is merely
  # to reduce flicker: 
  array set foo {
    h {} m {} s {}
    H 0 M 0 S 0
  }
  proc tick {} {
    global foo
    upvar #0 Config C

    set now [clock seconds]
    foreach {H M S} [split [clock format $now -format "%I.%M.%S"] .] break
    
    set sx $C(X,base) 
    set sy $C(Y,base)

    if {$H != $foo(H)} {
      set foo(H) $H
      foreach x $foo(h) {destroy $x}
      set foo(h) [drawTransNumber .trans $sx $sy $H]
    }
    incr sx $C(X,incr)
    incr sy $C(Y,incr)

    if {$M != $foo(M)} {
      set foo(M) $M
      foreach x $foo(m) {destroy $x}
      set foo(m) [drawTransNumber .trans $sx $sy $M]
    }
    incr sx $C(X,incr)
    incr sy $C(Y,incr)

    if {$S != $foo(S)} {
      set foo(S) $S
      foreach x $foo(s) {destroy $x}
      set foo(s) [drawTransNumber .trans $sx $sy $S]
    }
    after 1000 tick
  }
  tick

  # Show the coords, useful with the new move command:
  pack [frame .fy] -side top
  pack [label .fy.l -text "Y,base :"] -side left
  pack [label .fy.v -textvariable Config(Y,base)] -side right
  pack [frame .fx] -side top
  pack [label .fx.l -text "X,base :"] -side left
  pack [label .fx.v -textvariable Config(X,base)] -side right

  # To allow us to easily kill this beast:
  pack [button .exit -command exit -text "Exit"] -side right

  # Allow moving the clock:
  pack [button .move -command move -text "Move here"] -side left

  proc move {} {
    global Config foo
    
    foreach x [after info] {after cancel $x}
    array set foo {H 0 M 0 S 0}
    set Config(X,base) [winfo x .]
    set Config(Y,base) [winfo y .]
    tick
  }
