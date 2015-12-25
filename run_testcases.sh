#!/bin/bash
testcasePath=../testcases
if [ ! -d "testcasesResult" ];then
	mkdir testcasesResult
	echo "makedir testcasesResult"
fi

if [ $# -eq 0 ];then
	for dir in ${testcasePath}/* ; do
		#echo $dir
		name=${dir##*/}
		./scc ${dir}/${name}.sc ./testcasesResult/my${name}.pt ./testcasesResult/my${name}.ll
		if [ $? -eq 0 ];then
			echo -e "${name}\tsuccessed" 
		else
			echo -e "${name}\tfailed"
		fi
	done
fi

if [ $# -eq 1 ];then
	lli ./testcasesResult/$1.ll
fi
