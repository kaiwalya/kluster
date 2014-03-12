#/usr/bin/env python

import os
import sys
import docs

from subprocess import call

def findEnvironmentVariable(varKey, defaultValue = None):
	ret = defaultValue;
	if varKey in os.environ:
		val = os.environ[varKey]
		while val.startswith('"') and val.endswith('"'):
			val = val[1:-1]
		if len(val) > 0:
			ret = val
	return ret;

if __name__ == "__main__":

	projectDir = None
	builder = docs.Builder()

	outputDir = findEnvironmentVariable("OUTPUT_DIRECTORY");
	if outputDir is None or len(outputDir) == 0:
		builder.showError("Cannot find OUTPUT_DIRECTORY in environment")
		sys.exit(1)
	outputDir = os.path.abspath(outputDir)
	builder.showInfo("Output Directory:", outputDir)

	

	sourceDir = findEnvironmentVariable("SOURCE_DIRECTORY");
	if sourceDir is None or len(sourceDir) == 0:
		builder.showError("Cannot find SOURCE_DIRECTORY in environment")
		sys.exit(1)
	sourceDir = os.path.abspath(sourceDir)
	builder.showInfo("Source Directory:", sourceDir)


	action = None;
	if "ACTION" in os.environ:
		action = os.environ["ACTION"]
	if action is None or len(action) == 0:
		action = "build"
	builder.showInfo("Action:", action)


	ret = 1;

	if action == "build":
		ret = builder.build(outputDir, sourceDir)
	elif action == "clean":
		ret = builder.clean(outputDir);
	else:
		builder.showError("Didnot understand action: \"" + action + "\"")
	sys.exit(ret)