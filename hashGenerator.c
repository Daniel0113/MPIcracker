unsigned long hash(unsigned char *str) {
    unsigned long hash = 5381;
    int c;

    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }

    return hash;
} // http://www.cse.yorku.ca/~oz/hash.html


int main(int argc, char *argv[]) {
    printf("String: %s, Hash: %lu\n", argv[1], hash(argv[1]));
}