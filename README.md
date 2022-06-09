# final-project-tm_sf_jm
final-project-tm_sf_jm created by GitHub Classroom

## Traces Location
```
~/final-project-tm_sf_jm/traces/
```

## How to Compile
Use the following command to compile the program.
```
cd ~/final-project-tm_sf_jm
make clean
make
```
## How to Run
From inside the `final-project-tm_sf_jm` directory run the following commands to run different predictors (these commands are example.
Change the parameters according to your need):

### Parameters:
```
a: address bits size (default: 8)
m: last load value history instances  
   (i.e., m = 3 keeps 10 12 14 last values of a value as history to predict,
   this option is only given for choice 7, default: 1)
n: LCT bits (valid inputs for n = 1, 2 i.e., n = 2 keeps two bits for prediction states)
s: stride (for choiche 2-5 and 8) 
p: choice of predictor (i.e., p = 8 produces result for tournament load value predictor, default: 8)
```

## Predictors
### Last Value (p = 0)
Example command:
```
./lvpsim -i traces/pipesim1.out -a 8  -p 0
```
Exact commands to generate our results:
```
./lvpsim -i traces/pipesim1.out -a 8 -p 0
./lvpsim -i traces/pipesim2.out -a 8 -p 0
./lvpsim -i traces/pipesim3.out -a 8 -p 0
```
### Last Value Approach With LCT (p = 1)
Example command:
```
./lvpsim -i traces/pipesim1.out -a 8 -n 2 -p 1
```
Exact commands to generate our results:
```
./lvpsim -i traces/pipesim1.out -n 1 -a 8 -p 1
./lvpsim -i traces/pipesim1.out -n 2 -a 8 -p 1
./lvpsim -i traces/pipesim2.out -n 1 -a 8 -p 1
./lvpsim -i traces/pipesim2.out -n 2 -a 8 -p 1
./lvpsim -i traces/pipesim3.out -n 1 -a 8 -p 1
./lvpsim -i traces/pipesim3.out -n 2 -a 8 -p 1
```

### Last Value + Stride (p = 2)
Example command:
```
./lvpsim -i traces/pipesim1.out -a 8 -s 1 -p 2
```
Exact commands to generate our results:
```
./lvpsim -i traces/pipesim1.out -a 8 -p 2
./lvpsim -i traces/pipesim2.out -a 8 -p 2
./lvpsim -i traces/pipesim3.out -a 8 -p 2
```

### Last Value + Stride with LCT (p = 3)
Example command:
```
./lvpsim -i traces/pipesim1.out -a 8 -n 2 -s 1 -p 3
```
Exact commands to generate our results:
```
./lvpsim -i traces/pipesim1.out -n 2 -a 8 -p 3
./lvpsim -i traces/pipesim2.out -n 2 -a 8 -p 3
./lvpsim -i traces/pipesim3.out -n 2 -a 8 -p 3
```

### FCM: Learning Stride (p = 4)
Example command:
```
./lvpsim -i traces/pipesim1.out -a 8 -s 1 -p 4
```
Exact commands to generate our results:
```
./lvpsim -i traces/pipesim1.out -a 8 -p 4
./lvpsim -i traces/pipesim2.out -a 8 -p 4
./lvpsim -i traces/pipesim3.out -a 8 -p 4
```
### FCM: Learning Stride (with LCT) (p = 5)
Example command:
```
./lvpsim -i traces/pipesim1.out -a 8 -n 2 -s 1 -p 5
```
Exact commands to generate our results:
```
./lvpsim -i traces/pipesim1.out -n 2 -a 8 -p 5
./lvpsim -i traces/pipesim2.out -n 2 -a 8 -p 5
./lvpsim -i traces/pipesim3.out -n 2 -a 8 -p 5
```

### PC + memory address indexing (with LCT) (p = 6)
Example command:
```
./lvpsim -i traces/pipesim1.out -a 8 -n 2 -p 6
```
Exact commands to generate our results:
```
./lvpsim -i traces/pipesim1.out -n 2 -a 8 -p 6
./lvpsim -i traces/pipesim2.out -n 2 -a 8 -p 6
./lvpsim -i traces/pipesim3.out -n 2 -a 8 -p 6
```

### FCM wih Maximum Occurrence (p = 7)
Example command:
```
./lvpsim -i traces/pipesim3.out -m 2 -n 2 -a 8 -p 7
```
Exact commands to generate our results:
```
./lvpsim -i traces/pipesim1.out —m 2 -n 2 -a 8 -p 7
./lvpsim -i traces/pipesim2.out —m 2 -n 2 -a 8 -p 7
./lvpsim -i traces/pipesim3.out -m 2 -n 2 -a 8 -p 7
```
### Tournament Predictor (p = 8)
Example command:
```
./lvpsim -i traces/pipesim1.out -a 8 -n 2 -s 1 -p 8
```
Exact commands to generate our results:
```
./lvpsim -i traces/pipesim1.out -n 2 -a 8 -p 8
./lvpsim -i traces/pipesim2.out -n 2 -a 8 -p 8
./lvpsim -i traces/pipesim3.out -n 2 -a 8 -p 8
```
