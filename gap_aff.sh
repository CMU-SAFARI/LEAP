#for i in bowtie2_augment/*_pairs; do 
#    echo $i; 
#    for j in {0..2}; do 
#        echo $j
#        ./testNW 6 $j < $i 
#    done
#done

# NW across benchmarks across 1..5 errors 
echo "" >> gap_aff.txt 
echo "NW 1 across 1..5" >> gap_aff.txt 
i=./bowtie2_augment/ERR240726_1.filt.fastq_pairs
for j in {1..5}; do
    echo $j >> gap_aff.txt 
    ./testNW $j 1 < $i >> gap_aff.txt 
done

echo "NW 2 across 1..5" >> gap_aff.txt 
#i=./bowtie2_augment/ERR240726_1.filt.fastq_pairs
for j in {1..5}; do
    echo $j >> gap_aff.txt 
    ./testNW $j 2 < $i >> gap_aff.txt 
done

#echo "LV across 1..5" >> gap_aff.txt 
#for j in {1..5}; do
#    echo $j >> gap_aff.txt
#    ./vectorLV $j < $i >> gap_aff.txt 
#done 

echo "BV LEAP" >> gap_aff.txt
for j in {1..5}; do
    echo $j >> gap_aff.txt
    ./vectorSHD_ED $j < $i >> gap_aff.txt 
done

echo "nonBV LEAP" >> gap_aff.txt 
for j in {1..5}; do
    echo $j >> gap_aff.txt
    ./testLV $j < $i >> gap_aff.txt
done 


#echo "seqan" >> gap_aff.txt 
#for j in {1..5}; do
#    echo $j >> gap_aff.txt
#    /DATA/seqanChecker $j < $i >> gap_aff.txt 
#done

# Skip_ED across benchmarks across 1..5 errors 

##i=./bowtie2_augment/ERR240726_1.filt.fastq_pairs
##echo $i
##for j in {1..5}; do
##    echo $j 
##    #echo $(basename "$i")_$j_pair >> graduate_hours 
##    
##    echo $(basename "$i")_$j 
##    ./vectorSHD_ED $j < $i > $(basename "$i")_$j
##done


