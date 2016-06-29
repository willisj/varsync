# varsync

Example output

=====================
|||   WINDOW 1
|||
=====================

@raspberrypi ~/Documents/varsync $ ./bin/varsync-test server send                                                                                                                   
Initializing list                                                                                                                                                                   
Waiting for connection                                                                                                                                                              
                [Done]                                                                                                                                                              
Initializing variable                                                                                                                                                               
                [Done]                                                                                                                                                              
Adding variable to list                                                                                                                                                             
                [Done]                                                                                                                                                              
Creating thread                                                                                                                                                                     
                [Done]                                                                                                                                                              
Var: 0                                                                                                                                                                              
Var: 1                                                                                                                                                                              
Var: 2                                                                                                                                                                              
Var: 3                                                                                                                                                                              
Var: 4                                                                                                                                                                              
Var: 5                                                                                                                                                                              
Var: 6                                                                                                                                                                              
Var: 7                                                                                                                                                                              
Var: 8                                                                                                                                                                              
Var: 9                                                                                                                                                                              
Var: 10                                                                                                                                                                             
Var: 11                                                                                                                                                                             
Var: 12                                                                                                                                                                             
Var: 13                                                                                                                                                                             
Var: 14                                                                                                                                                                             
Var: 15                                                                                                                                                                             
Var: 16                                                 

=====================
|||   WINDOW 2 
|||
=====================

pi@raspberrypi ~/Documents/varsync $ ./bin/varsync-test client recv
Initializing list
                [Done]
Initializing variable
                [Done]
Adding variable to list
                [Done]
Creating thread
                [Done]
{ "id" : 1,  "sz" : 4, "val": 1}
{ "id" : 2,  "sz" : 39, "val": "7468697320697320736f6d652073616d706c65206461746120666f722074686520627566666572"}
{ "id" : 1,  "sz" : 4, "val": 1}
{ "id" : 2,  "sz" : 39, "val": "7468697320697320736f6d652073616d706c65206461746120666f722074686520627566666572"}
{ "id" : 1,  "sz" : 4, "val": 3}
{ "id" : 2,  "sz" : 39, "val": "7468697320697320736f6d652073616d706c65206461746120666f722074686520627566666572"}
{ "id" : 1,  "sz" : 4, "val": 3}
{ "id" : 2,  "sz" : 39, "val": "7468697320697320736f6d652073616d706c65206461746120666f722074686520627566666572"}
{ "id" : 1,  "sz" : 4, "val": 5}
{ "id" : 2,  "sz" : 39, "val": "7468697320697320736f6d652073616d706c65206461746120666f722074686520627566666572"}
{ "id" : 1,  "sz" : 4, "val": 5}
{ "id" : 2,  "sz" : 39, "val": "7468697320697320736f6d652073616d706c65206461746120666f722074686520627566666572"}
{ "id" : 1,  "sz" : 4, "val": 7}
{ "id" : 2,  "sz" : 39, "val": "7468697320697320736f6d652073616d706c65206461746120666f722074686520627566666572"}
{ "id" : 1,  "sz" : 4, "val": 7}
{ "id" : 2,  "sz" : 39, "val": "7468697320697320736f6d652073616d706c65206461746120666f722074686520627566666572"}
{ "id" : 1,  "sz" : 4, "val": 9}
{ "id" : 2,  "sz" : 39, "val": "7468697320697320736f6d652073616d706c65206461746120666f722074686520627566666572"}
{ "id" : 1,  "sz" : 4, "val": 9}
{ "id" : 2,  "sz" : 39, "val": "7468697320697320736f6d652073616d706c65206461746120666f722074686520627566666572"}
{ "id" : 1,  "sz" : 4, "val": 11}
{ "id" : 2,  "sz" : 39, "val": "7468697320697320736f6d652073616d706c65206461746120666f722074686520627566666572"}
{ "id" : 1,  "sz" : 4, "val": 11}
{ "id" : 2,  "sz" : 39, "val": "7468697320697320736f6d652073616d706c65206461746120666f722074686520627566666572"}
{ "id" : 1,  "sz" : 4, "val": 13}
{ "id" : 2,  "sz" : 39, "val": "7468697320697320736f6d652073616d706c65206461746120666f722074686520627566666572"}
{ "id" : 1,  "sz" : 4, "val": 13}
{ "id" : 2,  "sz" : 39, "val": "7468697320697320736f6d652073616d706c65206461746120666f722074686520627566666572"}
{ "id" : 1,  "sz" : 4, "val": 15}
{ "id" : 2,  "sz" : 39, "val": "7468697320697320736f6d652073616d706c65206461746120666f722074686520627566666572"}
{ "id" : 1,  "sz" : 4, "val": 15}
{ "id" : 2,  "sz" : 39, "val": "7468697320697320736f6d652073616d706c65206461746120666f722074686520627566666572"}
{ "id" : 1,  "sz" : 4, "val": 17}
{ "id" : 2,  "sz" : 39, "val": "7468697320697320736f6d652073616d706c65206461746120666f722074686520627566666572"}
{ "id" : 1,  "sz" : 4, "val": 17}
{ "id" : 2,  "sz" : 39, "val": "7468697320697320736f6d652073616d706c65206461746120666f722074686520627566666572"}
{ "id" : 1,  "sz" : 4, "val": 17}
{ "id" : 2,  "sz" : 39, "val": "7468697320697320736f6d652073616d706c65206461746120666f722074686520627566666572"}
