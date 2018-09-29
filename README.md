Arduino example code for Open drain Power Board.

The code is for relay board testing purpose.

Attention should be paid to:

#define NUMBER_OF_EXPANDERS 1 - The i2c address starts from 0x20 and finish at 0x27. So if you change it from 1 to 3 the board would have addresses like this: 1- 0x20, 2- 0x21, 3- 0x22
