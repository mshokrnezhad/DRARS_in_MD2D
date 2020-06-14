#!/bin/bash

g++ -g relay_selection.cpp -o rese.out
g++ -g reqirement_satisfaction.cpp -o rqsa.out
g++ -g resource_allocation.cpp -o ra.out

dr=3;
cr=3;
tsir=0.1;
max_power=10000;
noise=1;

for k in `seq 3 3`;
do
     	for n in `seq 5 5`;
	do	
		echo "k=$k,n=$n" >> R03_cpp_dec_SoP.txt
		echo "k=$k,n=$n" >> R03_cpp_dec_SoS.txt
		#echo "k=$k,n=$n" >> R03_cpp_dec_OR.txt

		for itr in `seq 20 30`;
		do	
			echo "k=$k,n=$n,iteration=$itr"	
			#sleep 1
			
			if [ $itr -eq 50 ]
			then
				sed -n "/^k=$k,n=$n,iteration=50/,/^k=$k,n=$(($n + 1)),iteration=1/p" R01_LoN.txt | awk '/^x/'|sed 's/x\[0\] = 5000;//g'|sed '/^\s*$/d'|sed 's/;//g'|sed 's/x//g'|sed 's/ = / /g'|sed 's/\[//g'|sed 's/\]//g'>I01_x.txt
				echo "$(($n + 1)) 5000" >> I01_x.txt
				sed -n "/^k=$k,n=$n,iteration=50/,/^k=$k,n=$(($n + 1)),iteration=1/p" R01_LoN.txt | awk '/^y/'|sed 's/y\[0\] = 5000;//g'|sed '/^\s*$/d'|sed 's/;//g'|sed 's/y//g'|sed 's/ = / /g'|sed 's/\[//g'|sed 's/\]//g'>I01_y.txt
				echo "$(($n + 1)) 5000" >> I01_y.txt
			else
				sed -n "/^k=$k,n=$n,iteration=$itr$/,/^k=$k,n=$n,iteration=$(($itr + 1))$/p" R01_LoN.txt | awk '/^x/'|sed 's/x\[0\] = 5000;//g'|sed '/^\s*$/d'|sed 's/;//g'|sed 's/x//g'|sed 's/ = / /g'|sed 's/\[//g'|sed 's/\]//g'>I01_x.txt
			echo "$(($n + 1)) 5000" >> I01_x.txt
				sed -n "/^k=$k,n=$n,iteration=$itr$/,/^k=$k,n=$n,iteration=$(($itr + 1))$/p" R01_LoN.txt | awk '/^y/'|sed 's/y\[0\] = 5000;//g'|sed '/^\s*$/d'|sed 's/;//g'|sed 's/y//g'|sed 's/ = / /g'|sed 's/\[//g'|sed 's/\]//g'>I01_y.txt
			echo "$(($n + 1)) 5000" >> I01_y.txt
			fi

			./rese.out $n $noise $max_power
			./rqsa.out $n $dr $cr
			./ra.out $n $k $tsir $noise $max_power

			rm R01_NN.txt
			rm R01_TNN.txt
			rm R01_NoH.txt
			rm I01_x.txt
			rm I01_y.txt
		done 
	done    
done 

rm rese.out
rm rqsa.out
rm ra.out
