# ReviveUSBMicroMatrix16F1454
ReviveUSBMicro<br>
https://github.com/bit-trade-one/ADRVMIC-REVIVE-USB-Micro <br>
をPIC16F1454用に移植、コンパイラもXC8に対応 <br>
MLA v2018_11_26をベースにReviveUSBに必要な部分を書き換えてます<br>
<br>
加えて<br>
・ポートの読み取り、初期化部分を別ファイル(User.c)に<br>
・EEPROMの代わりにHEFを利用<br>
・RA3,4,5は内蔵プルアップを有効に<br>
