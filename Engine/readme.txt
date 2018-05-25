Adjusting how the engine play

You can create a textfile in the personalities subfolder to the engine and name it <Personality>.per.
When the engine start it will look in the personality subfolder for personalities.
If it find a personality with the name Noraml.per it wil load this and use it as defalt personality.
The other files it find wil be put into the option style list and can be selected by the user.
You can give any legal command in these files but normaly you use this to alter the engine with changing
the evaluation parametre.

Format:
eval <parameter> <value>

Parameters
elo <n>
The engine will play at a fixed strength.

strength <n.n>
Strength given in % of full strength <0.00-100.00>. This feature is for testing of the engine use 'elo <n>' for normal use.
 
pawn <n>
Change the value of a pawn in centipawn.

knight <n>
Change the value of a knight in centipawn.

bishop <n>
Change the value of a bishop in centipawn.

rook <n>
Change the value of a rook in centipawn.

queen <n>
Change the value of a queen in centipawn.

contempt <n>
Set to a high values will try to avoid draws. Negative valuse will make it prefere draws.