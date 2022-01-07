#!/usr/bin/python
# -*- coding: utf-8 -*-

# Distribuido bajo licencia GNU/GPL 3
# Contacto: Gustavo Díaz, GDNet Projects - www.gdnet.com.ar | Email: gdiaz@gdnet.com.ar

# Módulos de PyQt
from PyQt4.QtCore import Qt, SIGNAL, QSize, QString, QTimer
from PyQt4.QtGui import QWidget, QDesktopWidget, QPainter, QImage, qApp
from PyQt4.QtSvg import QSvgRenderer

# Importamos el widget del Login
from gui.Ui_PausedWidget import Ui_PausedWidget
# Importamos el módulo de skinning
from core.skinning.skinning import get_images

# Módulo adm. de sesión
from core.session.session import Session

# Adm. de base de datos
from core.db.dbMain import createHashedPassword

DEBUG = False # Sólo para usar en modo desarrollo

# La session es singleton con lo cual puede usarse como global sin problemas
sessionObject = Session()

# Clase que coloca un widget al estar pausado el Puesto
class PausedWidget (QWidget, Ui_PausedWidget):
	def __init__(self, parent=None):
		QWidget.__init__(self, parent)
		self.setupUi(self)
		
		# Llamamos al método que carga las imágenes de la aplicación
		self.loadImages()
		
		if not DEBUG:
			# Eliminamos los bordes de ventana
			self.setWindowFlags(Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint | Qt.Tool)
			
		# La ventana de iniciará en modo FullScreen
		self.setWindowState(Qt.WindowFullScreen)
		
		# Posicionamos el frame del Login en el centro del Widget
		screen = QDesktopWidget().screenGeometry()
		size =  self.geometry()
		self.pausedWidgetContainer.move((screen.width() - size.width()) / 2 , (screen.height() - size.height()) / 2 - 50)
		
		# Fondo del Widget principal del login
		# Establecemos la imágen SVG
		self.widgetBgSVG = QSvgRenderer(QString(self.loginWidgetSvg))
		# Creamos el formato dela imagen para pintar el fondo en el widget
		self.background = QImage(QSize(screen.width(), screen.height()), QImage.Format_ARGB32)
		self.painter = QPainter()
		self.painter.begin(self.background)
		self.widgetBgSVG.render(self.painter, QString("background"))
		self.painter.end()
		
		# Ocultamos el mensaje de error del login
		self.pausedLabel.hide()
		
		# Señal emitida al pesionar sobre el botón Continuar
		self.connect(self.continueButton, SIGNAL("clicked()"), self.resumeWorkstation)

	# Pintamos el fondo del login
	def paintEvent(self, event):
		self.painter = QPainter()
		self.painter.begin(self)
		self.painter.drawImage(0, 0, self.background)
		self.painter.end()
		self.arrowCursor = QCursor(Qt.ArrowCursor) 
		self.arrowCursor.setPos(10,10)
	# Método para la deshabilitación del estado Pausado
	def resumeWorkstation(self):
		password = createHashedPassword(self.passwdLineEdit.text())
		if sessionObject['client'].password == password:
			self.emit(SIGNAL("resumeWorkStation()"))
			# Limpiamos el password para que  no quede guardado
			self.passwdLineEdit.clear()
			self.hide()
		else:
			self.passwdLabelMsg(qApp.translate("PausedWidget", "La contraseña no es válida.", None, qApp.UnicodeUTF8), True)
	
	# Método para mostrar el Error 
	def passwdLabelMsg(self,  msg, timer):
		self.passwdLineEdit.clear()
		self.pausedLabel.setText(msg)
		self.pausedLabel.show()
		if timer:
			self.errorTimer = QTimer()
			self.errorTimer.start(2000)
			self.connect(self.errorTimer, SIGNAL("timeout()"), self.pausedLabel.hide)

	# Evitamos cerrar el app con Alt + F4
	def closeEvent(self, event):
		event.ignore()

	# Método que carga la apariencia de la aplicación
	def loadImages(self):
		self.loginWidgetSvg = get_images()["loginWidgetBg"]
		self.pausedWidgetLogo.setPixmap(get_images()["loginLogo"])
		self.continueButton.setIcon(get_images()["button_login"])
