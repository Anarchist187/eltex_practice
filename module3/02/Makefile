all: setup

setup: prepare

prepare:
			gcc interpret.c -o interpret
			gcc maxval.c -o maxval
			gcc sum.c -o sum
			gcc unitestr.c -o unitestr	

clear:
			rm interpret || true
			rm maxval || true
			rm sum || true
			rm unitestr