Following arguments need to be passed to run the program :

1.One level branch Prediction

	./predictor.out 1 2 (second arguments defines bit counter so it can be varied from 2, 4, 6, 8)

2.Two Level Global Branch Prediction

	./predictor.out 2 2 (second arguments defines bit counter so it can be varied from 2, 4, 6, 8)
3.Two Level Gshare Branch Predictor

	./predictor.out 3 2 (second arguments defines bit counter so it can be varied from 2, 4, 6, 8)

4.Two Level Local Branch Prediction

	./predictor.out 4 2 (second arguments defines bit counter so it can be varied from 2, 4, 6, 8)


To run q3.c following command:

	./q3.out 3 2 1 -  arg1 - Gshare,  arg2 - n-bit counter , arg3 - (1 represents drystone trace nad 2 represents linpack trace)

Tournament predictor is extraCredit.c

	./extraCredit.out 2 :- arg1 represents n-bit counter. 
