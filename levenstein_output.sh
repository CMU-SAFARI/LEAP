#for i in bowtie2_augment/*_pairs; do 
#    echo $i; 
#    for j in {0..2}; do 
#        echo $j
#        ./testNW 6 $j < $i 
#    done
#done

# NW across benchmarks across 1..5 errors 
echo "" >> levenstein_output.txt 
echo "NW 1 across 1..5" >> levenstein_output.txt 
i=./bowtie2_augment/ERR240726_1.filt.fastq_pairs
for j in {1..5}; do
    echo $j >> levenstein_output.txt 
    ./testNW $j 1 < $i >> levenstein_output.txt 
done

echo "NW 2 across 1..5" >> levenstein_output.txt 
i=./bowtie2_augment/ERR240726_1.filt.fastq_pairs
for j in {1..5}; do
    echo $j >> levenstein_output.txt 
    ./testNW $j 2 < $i >> levenstein_output.txt 
done

echo "LV across 1..5" >> levenstein_output.txt 
for j in {1..5}; do
    echo $j >> levenstein_output.txt
    ./vectorLV $j < $i >> levenstein_output.txt 
done 

echo "BV LEAP" >> levenstein_output.txt
for j in {1..5}; do
    echo $j >> levenstein_output.txt
    ./vectorSHD_ED $j < $i >> levenstein_output.txt 
done

echo "seqan" >> levenstein_output.txt 
for j in {1..5}; do
    echo $j >> levenstein_output.txt
    /DATA/seqanChecker $j < $i >> levenstein_output.txt 
done

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


