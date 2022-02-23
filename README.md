# examthing
Process SIMS 'csv' of exam timetable into something useful.

A brief tidy of old code to make SIMS do soemthing useful. Takes a file containing records like,
```
"Individual Candidate Timetable","","","","","","","","","","","","","","","","","","",""
"","","Season :","","","","","Spring Mocks 2022","","","","","","","","Centre Number :","12345","","",""
"","","Name :","","","","","","","","","","","","","","","","",""
"","","","","","","","Bloggs, Joe","","","","","","","","Year :","11","","",""
"","","Exam Number :","","","","","","","","","","","","","","","","",""
"","","","","","","","8000","","","","","","","","Reg Group :","11a bcd","","",""
"","","UCI :","","","","","","","","","","","","","","","","",""
"","","","","","","","123456789012Z","","","","","","","","ULN :","1234567890","","",""
"","Special Arrangements required","","","","","","","","","","","","","","","","","",""
"","","","","","Start ","","","","","Element ","","Component","","","","","","",""
"","","Date","","","","Board","","","Level","","Element Title","","","Component Title","","","Duration","Room","Seat"
"","","","","","Time","","","","","Code","","Code","","","","","","",""
"","","","","","","","","","","","","","","","","","","V1","A1        "
"","Mon 28 Feb","",""," 9:00AM","","DOM","","GCSE/FC","","RE02","RE GCSE","","RE02","RE GCSE Mock","","","2h 12m","",""
"","","","","","","","","","","","","","","","","","","V1","A1        "
"","Tue 01 Mar","",""," 9:00AM","","DOM","","GCSE/FC","","SCO1BH","Science Comb Biology Higher","","SCO1BH","Science Comb Biology Higher","","","1h 28m","",""
"","","","","","","","","","","","","","","","","","","V2","L1        "
"","Wed 02 Mar","",""," 9:00AM","","DOM","","GCSE/FC","","MaMoH","Maths Mock Higher","","MaHi1","Maths Higher Paper 1","","","1h 53m","",""
"","","","","","","","","","","","","","","","","","","V1","A1        "
"","Thu 03 Mar","",""," 9:00AM","","DOM","","GCSE/FC","","8700","English Language AQA 8700","","8700/1","English Language Mock","","","2h 12m","",""
"","","","","","","","","","","","","","","","","","","V1","A1        "
"","Fri 04 Mar","",""," 9:00AM","","DOM","","GCSE/FC","","SCO1CH","Science Comb Chemistry Higher","","SCO1CH","Science Comb Chemistry Higher","","","1h 28m","",""
"","","","","","","","","","","","","","","","","","","V2","L1        "
"","Mon 07 Mar","",""," 9:00AM","","DOM","","GCSE/FC","","MaMoH","Maths Mock Higher","","MaPaper2","Maths Higher Paper 2","","","1h 53m","",""
"","","","","","","","","","","","","","","","","","","V1","A1        "
"","Tue 08 Mar","",""," 9:00AM","","DOM","","GCSE/FC","","SCO1PH","Science Comb Physics Higher","","SCO1PH","Science Comb Physics Higher","","","1h 28m","",""
"","","","","","","","","","","","","","","","","","","V1","A1        "
"","Tue 08 Mar","","","12:45PM","","DOM","","GCSE/FC","","ComMD","Computing GCSE","","ComMD","Computing GCSE Mock","","","1h 53m","",""
"","","","","","","","","","","","","","","","","","","V1","A1        "
"","Wed 09 Mar","","","12:45PM","","DOM","","CNAT/1&2","","CNICT","Cambridge Nationals ICT","","CNICT","Cambridge Nationals ICT","","","1h 53m","",""
"","","","","","","","","","","","","","","","","","","V1","A1        "
"","Thu 10 Mar","",""," 9:00AM","","DOM","","GCSE/FC","","GraSS2","Graphics GCSE Mock","","GraSS2","DT GCSE Mock","","","1h 53m","",""
"","","Printed On","23/02/2022","","","","","","","","","","","","","","","",""
``
and turns it into
```
"Bloggs, Joe", 8000, 11, 11a bcd, Mon 28 Feb,  9:00AM, RE02, RE GCSE Mock, 2h 12m, V1, A1
"Bloggs, Joe", 8000, 11, 11a bcd, Tue 01 Mar,  9:00AM, SCO1BH, Science Comb Biology Higher, 1h 28m, V1, A1
"Bloggs, Joe", 8000, 11, 11a bcd, Wed 02 Mar,  9:00AM, MaMoH, Maths Higher Paper 1, 1h 53m, V2, L1
"Bloggs, Joe", 8000, 11, 11a bcd, Thu 03 Mar,  9:00AM, 8700, English Language Mock, 2h 12m, V1, A1
"Bloggs, Joe", 8000, 11, 11a bcd, Fri 04 Mar,  9:00AM, SCO1CH, Science Comb Chemistry Higher, 1h 28m, V1, A1
"Bloggs, Joe", 8000, 11, 11a bcd, Mon 07 Mar,  9:00AM, MaMoH, Maths Higher Paper 2, 1h 53m, V2, L1
"Bloggs, Joe", 8000, 11, 11a bcd, Tue 08 Mar,  9:00AM, SCO1PH, Science Comb Physics Higher, 1h 28m, V1, A1
"Bloggs, Joe", 8000, 11, 11a bcd, Tue 08 Mar, 12:45PM, ComMD, Computing GCSE Mock, 1h 53m, V1, A1
"Bloggs, Joe", 8000, 11, 11a bcd, Wed 09 Mar, 12:45PM, CNICT, Cambridge Nationals ICT, 1h 53m, V1, A1
"Bloggs, Joe", 8000, 11, 11a bcd, Thu 10 Mar,  9:00AM, GraSS2, DT GCSE Mock, 1h 53m, V1, A1
```
