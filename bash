#!/bin/bash

./clientApp hileci1 C 50 127.0.0.1:5555 & ./clientApp hileci2 C 50 127.0.0.1:5555 & ./clientApp hileci3 Q 65 127.0.0.1:5555 & ./clientApp hileci4 T 75 127.0.0.1:5555 & ./clientApp hileci5 Q 15 127.0.0.1:5555
 
