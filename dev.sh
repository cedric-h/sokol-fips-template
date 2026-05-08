find ./src | \
    grep -E "[^.swp|.o|.d|.DS_Store]$" | \
    entr -rs './fips make eab && ./fips run eab'
