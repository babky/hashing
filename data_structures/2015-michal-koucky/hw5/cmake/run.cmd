gen-data.exe -s 1 I C | solution.exe cuckoo tab > ict.raw

gen-data.exe -s 1 I | solution.exe linear mult > ilm.raw
gen-data.exe -s 1 I | solution.exe linear tab > ilt.raw

gen-data.exe -s 1 I | solution.exe two mult > itm.raw
gen-data.exe -s 1 I | solution.exe two tab > itt.raw

gen-data.exe -s 1 F C | solution.exe cuckoo mult > fcm.raw
gen-data.exe -s 1 F C | solution.exe cuckoo tab > fct.raw

gen-data.exe -s 1 F | solution.exe linear mult > flm.raw
gen-data.exe -s 1 F | solution.exe linear tab > flt.raw

gen-data.exe -s 1 F | solution.exe two mult > ftm.raw
gen-data.exe -s 1 F | solution.exe two tab > ftt.raw
