 
for file in `find $1 -name "*.cpp"`
do
mv $file $file.bak
more $file.bak | sed 's#\"\./#\"#g' > $file
rm $file.bak
done 

for file in `find $1 -name "*.h"`
do
mv $file $file.bak
more $file.bak | sed 's#\"\./#\"#g' > $file
rm $file.bak
done 


