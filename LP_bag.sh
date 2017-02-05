i=ERR240726_1.filt.fastq_pairs_5_repeat

#echo "BV LEAP SHD OFF" >> LP_bag_output.txt 
#for j in {1..5}; do
#    echo $j >> LP_bag_output.txt 
#    ./vectorSHD_ED $j < $i >> LP_bag_output.txt
#done
#
#echo "NW 1 BAG" >> LP_bag_output.txt
#for j in {1..5}; do
#    echo $j >> LP_bag_output.txt 
#    ./testNW $j 1 < $i >> LP_bag_output.txt
#done
#
#echo "NW 2 BAG" >> LP_bag_output.txt
#for j in {1..5}; do
#    echo $j >> LP_bag_output.txt
#    ./testNW $j 2 < $i >> LP_bag_output.txt
#done

echo "BV LEAP SHD ON" >> LP_bag_output.txt 
for j in {1..5}; do
    echo $j >> LP_bag_output.txt 
    ./vectorSHD_ED $j < $i >> LP_bag_output.txt
done
