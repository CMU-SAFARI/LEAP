
## NW across benchmarks across 1..5 errors 
#echo "LP_levenstein" >> LP_levenstein_output.txt 
#echo "NW 1 across 1..5" >> LP_levenstein_output.txt 
i=ERR240726_1.filt.fastq_pairs_5_repeat
#for j in {1..5}; do
#    echo $j >> LP_levenstein_output.txt 
#    ./testNW $j 1 < $i >> LP_levenstein_output.txt 
#done
#
#echo "NW 2 across 1..5" >> LP_levenstein_output.txt 
#for j in {1..5}; do
#    echo $j >> LP_levenstein_output.txt 
#    ./testNW $j 2 < $i >> LP_levenstein_output.txt 
#done
#
#echo "LV across 1..5" >> LP_levenstein_output.txt 
#for j in {1..5}; do
#    echo $j >> LP_levenstein_output.txt
#    ./vectorLV $j < $i >> LP_levenstein_output.txt 
#done 
#
#echo "BV LEAP" >> LP_levenstein_output.txt
#for j in {1..5}; do
#    echo $j >> LP_levenstein_output.txt
#    ./vectorSHD_ED $j < $i >> LP_levenstein_output.txt 
#done
#
#echo "seqan" >> LP_levenstein_output.txt 
#for j in {1..5}; do
#    echo $j >> LP_levenstein_output.txt
#    /DATA/seqanChecker $j < $i >> LP_levenstein_output.txt 
#done

echo "BV LEAP SHD ON" >> LP_levenstein_output.txt 
for j in {1..5}; do
    echo $j >> LP_levenstein_output.txt 
    ./vectorSHD_ED $j < $i >> LP_levenstein_output.txt
done

echo "LEAP NO BV" >> LP_levenstein_output.txt
for j in {1..5}; do 
    echo $j >> LP_levenstein_output.txt
    ./testLV $j < $i >> LP_levenstein_output.txt 
done 


