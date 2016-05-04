#! /bin/sh

# Test suite for rnote

echo "Reseting test directory..."
./reset.sh

echo "-- Version --"
echo "> -v"
./rnote -v

echo "-- List --"
echo "> -l"
./rnote -l
echo "> -l -v"
./rnote -l -v
echo "> -l okay"
./rnote -l okay

echo "-- Delete --"
echo "> -d"
./rnote -d
echo "> -d four three WOOPS!"
./rnote -d four three WOOPS!

echo "...weird..."
echo "> four WOOPS! -d three"
./rnote four WOOPS! -d three

echo "-- Bad Options --"
echo "> -X"
./rnote -X
echo "> -v -d"
./rnote -v -d
echo "> -d -v"
./rnote -d -v
echo "> -d -v -l"
./rnote -d -v -l
echo "./rnote -v \"this is a multiword inline note\""
./rnote -v "this is a multiword inline note"
echo "./rnote -"
./rnote -

echo "-- Inline --"
echo "./rnote \"this is a multiword inline note\""
./rnote "this is a multiword inline note"
echo "./rnote \"not this\""
./rnote "not this"
echo "./rnote \"and the third\""
./rnote "and the third"
echo "./rnote \"try to write a second inline note\" \"and it will fail\""
./rnote "try to write a second inline note\" \"and it will fail\""

echo "-- Inline Title Overflow Control --"
echo "./rnote \"__string of more than 100 characters__\""
./rnote "aaaaaaaaaabbbbbbbbbbccccccccccdddddddddd eeeeeeeeeeffffffffffgggggggggg hhhhhhhhhhiiiiiiiiiijjjjjjjjjjkkk phew! baby"
echo "./rnote \"__even bigger string of well-more-than 100 characters__\""
./rnote "aaaaaaaaaabbbbbbbbbbccccccccccdddddddddd eeeeeeeeeeffffffffffgggggggggg hhhhhhhhhhiiiiiiiiiijjjjjjjjjjkkk phew! baby lets see if we can break this thing further than we otherwise normaly would"

echo "-- Titled Note --"
echo "./rnote okay"
./rnote okay
echo "./rnote okay then"
./rnote okay then

echo "-- Main Note --"
./rnote