using namespace std;
#include <iostream>
#include <cstring>
#include <fstream>

struct suffix  {
    int index;
    char *suf;
};

int compareSuffix(struct suffix a, struct suffix b) {
    return strcmp(a.suf, b.suf) < 0? 1 : 0;
}

int* suffixArray(char* txt) {
    int n = strlen(txt);
    struct suffix suffixes[n];

    for (int i = 0; i < n; i++) {
        suffixes[i].index = i;
        suffixes[i].suf = (txt+i);
    }

    sort(suffixes, suffixes+n, compareSuffix);

    int *suffixArr = new int[n];
    for (int i = 0; i < n; i++)
        suffixArr[i] = suffixes[i].index;

    return  suffixArr;
}

int main() {
    string texto = "";
    string line;
    ifstream src("text.txt");
    int contLinha = 0;
        while(getline(src, line)) { //vai checar a linha para cada padrão
            if (contLinha == 0) {
                texto = texto + line;
            }
            else {
                texto = texto +"\n"+ line;
            }
            contLinha++;
        }

    char *txt = &texto[0u];
    int *a = suffixArray(txt);
    int n = strlen(txt);
    cout << n << "\n";
    cout << txt << "\n";
    for (int i = 0; i < n; i++) {
        cout << a[i] << "\n";
    }
    return 0;
}
