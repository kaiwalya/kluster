import io
import os
import sys
import inspect
import tempfile
from subprocess import call

class Builder:
	'''Platform independent logic for document build actions like "build", "clean", etc.'''

	def _whereAmI(self):
		return os.path.abspath(inspect.currentframe().f_back.f_back.f_globals["__file__"]) + ":" + str(inspect.currentframe().f_back.f_back.f_lineno) + ":"

	def showError(self, *args):
		'''
		Generate something that looks like this...
		/Users/k/dev/kluster/src/docs/docs.py:6: error: Blah
		'''
		print(self._whereAmI(), "error:", *args, file=sys.stderr)

	def showWarning(self, *args):
		'''
		Generate something that looks like this...
		/Users/k/dev/kluster/src/docs/docs.py:6: warning: Blah
		'''
		print(self._whereAmI(), "warning:", *args, file=sys.stderr)

	def showInfo(self,*args):
		'''
		Generate something that looks like this...
		/Users/k/dev/kluster/src/docs/docs.py:6: note: Blah
		'''
		print(self._whereAmI(), "note:", *args, file=sys.stderr)		

	
	def build(self, outputDir, sourceDir):
		'''
		Build action, calls doxygen
		'''

		self.showInfo("Source Directory:", sourceDir)
		self.showInfo("Output Directory:", outputDir)

		fDoxy = io.FileIO("Doxyfile")
		fDoxyData = fDoxy.readall()
		fDoxy.close()
		
		appendData = "OUTPUT_DIRECTORY = %(outputDir)s" % {"outputDir": outputDir}
		appendData = appendData + "\n"
		appendData = appendData + "INPUT = %(sourceDir)s" % {"sourceDir": sourceDir}
		appendData = appendData + "\n"
		appendData = appendData.encode('UTF-8')


		fTemp = tempfile.TemporaryFile()

		self.showInfo(fTemp)
		fTemp.write(fDoxyData)
		fTemp.write(appendData)
		fTemp.flush()
		fTemp.seek(0)

		#fDebug = open('test.txt', "w")
		#fDebug.write(fTemp.raw.readall().decode('utf-8'))
		#fDebug.flush()
		#fDebug.close()
		#fTemp.seek(0)
		
		call(["doxygen", "-"], stdin = fTemp)
		fTemp.close()
		return 0

	def clean(self):
		'''Clean actions, removes the generated output files'''
		return 0