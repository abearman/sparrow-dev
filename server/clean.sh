# Removes temp files/compiled python files in directory and subdirectories
find . -name "*.pyc" -exec rm -rf {} \;
find . -name "*.*~" -exec rm -rf {} \;