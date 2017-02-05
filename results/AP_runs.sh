apath=..
#i=${apath}/bowtie2_augment/ERR240726_1.filt.fastq_pairs
i=${apath}/ERR240726_1.filt.fastq_pairs_5
filename=LP_levenstein.txt


echo "L PAIRS LEVENSHTEIN" >> $filename 
echo "testLV" >> $filename
for j in {1..5}; do
    echo $j >> $filename
    ${apath}/testLV $j < $i >> $filename
done

echo "BV LEAP" >> $filename
for j in {1..5}; do
    echo $j >> $filename 
    ${apath}/vectorSHD_ED $j 0 1 < $i >> $filename 
done

echo "VectorLV" >> $filename
for j in {1..5}; do
    echo $j >> $filename 
    ${apath}/vectorLV $j < $i >> $filename 
done

echo "NW 1 across 1" >> $filename
for j in {1..1}; do
    echo $j >> $filename
    ${apath}/testNW $j 1 0 1 < $i >> $filename
done

echo "NW 2 across 1" >> $filename
for j in {1..1}; do
    echo $j >> $filename
    ${apath}/testNW $j 2 0 1 < $i >> $filename
done

echo "seqan" >> $filename
for j in {1..5}; do
    echo $j >> $filename
    /DATA/seqanChecker $j < $i >> $filename
done

echo "BV LEAP SHD ON" >> $filename
for j in {1..5}; do
    echo $j >> $filename 
    ${apath}/vectorSHD_ED $j 1 1 < $i >> $filename 
done



filename=LP_BAG.txt 
echo "L PAIRS BAG" >> $filename 
echo "BV LEAP" >> $filename
for j in {1..5}; do
    echo $j >> $filename 
    ${apath}/vectorSHD_ED $j 0 0 < $i >> $filename 
done

echo "NW 1 across 1" >> $filename
for j in {1..1}; do
    echo $j >> $filename
    ${apath}/testNW $j 1 0 0 < $i >> $filename
done

echo "NW 2 across 1" >> $filename
for j in {1..1}; do
    echo $j >> $filename
    ${apath}/testNW $j 2 0 0 < $i >> $filename
done

echo "BV LEAP SHD ON" >> $filename
for j in {1..5}; do
    echo $j >> $filename 
    ${apath}/vectorSHD_ED $j 1 0 < $i >> $filename 
done


