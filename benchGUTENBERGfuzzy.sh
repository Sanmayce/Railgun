# Run first $ su

lscpu
dmidecode --type 1
dmidecode --type 17

# Set CPU to performance mode for all CPUs
echo "Setting CPU to performance mode..."
for cpu in /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor; do
    echo performance | tee $cpu > /dev/null
done

cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor

echo -n "Quixote">needle
echo
nice -n -20 ./fuzzorama "gutenberg_en_all_2023-08_132000-html-files.tar" needle 2

echo -n "Schatzhauser">needle
echo
nice -n -20 ./fuzzorama "gutenberg_en_all_2023-08_132000-html-files.tar" needle 1

echo -n "Schatzhauser">needle
echo
nice -n -20 ./fuzzorama "gutenberg_en_all_2023-08_132000-html-files.tar" needle 2

echo -n "Schatzhauser">needle
echo
nice -n -20 ./fuzzorama "gutenberg_en_all_2023-08_132000-html-files.tar" needle 3

echo -n "Schatzhauser">needle
echo
nice -n -20 ./fuzzorama "gutenberg_en_all_2023-08_132000-html-files.tar" needle 4

echo -n "Gutenberg">needle
echo
nice -n -20 ./fuzzorama "gutenberg_en_all_2023-08_132000-html-files.tar" needle 1

echo -n "Gutenberg">needle
echo
nice -n -20 ./fuzzorama "gutenberg_en_all_2023-08_132000-html-files.tar" needle 2

echo -n "Gutenberg">needle
echo
nice -n -20 ./fuzzorama "gutenberg_en_all_2023-08_132000-html-files.tar" needle 3

echo Now, single-threaded...
echo

echo -n "Quixote">needle
echo
OMP_NUM_THREADS=1 ./fuzzorama "gutenberg_en_all_2023-08_132000-html-files.tar" needle 2

echo -n "Schatzhauser">needle
echo
OMP_NUM_THREADS=1 ./fuzzorama "gutenberg_en_all_2023-08_132000-html-files.tar" needle 1

echo -n "Schatzhauser">needle
echo
OMP_NUM_THREADS=1 ./fuzzorama "gutenberg_en_all_2023-08_132000-html-files.tar" needle 2

echo -n "Schatzhauser">needle
echo
OMP_NUM_THREADS=1 ./fuzzorama "gutenberg_en_all_2023-08_132000-html-files.tar" needle 3

echo -n "Schatzhauser">needle
echo
OMP_NUM_THREADS=1 ./fuzzorama "gutenberg_en_all_2023-08_132000-html-files.tar" needle 4

echo -n "Gutenberg">needle
echo
OMP_NUM_THREADS=1 ./fuzzorama "gutenberg_en_all_2023-08_132000-html-files.tar" needle 1

echo -n "Gutenberg">needle
echo
OMP_NUM_THREADS=1 ./fuzzorama "gutenberg_en_all_2023-08_132000-html-files.tar" needle 2

echo -n "Gutenberg">needle
echo
OMP_NUM_THREADS=1 ./fuzzorama "gutenberg_en_all_2023-08_132000-html-files.tar" needle 3

