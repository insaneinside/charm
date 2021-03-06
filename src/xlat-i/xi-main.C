#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "xi-symbol.h"
#include <string>
#include <list>

using std::cout;
using std::endl;

extern FILE *yyin;
extern void yyrestart ( FILE *input_file );
extern int yyparse (void);
extern int yyerror(char *);
extern int yylex(void);

extern xi::AstChildren<xi::Module> *modlist;

namespace xi {

#include "xi-grammar.tab.h"

/******************* Macro defines ****************/
class MacroDefinition {
public:
  char *key;
  char *val;
  MacroDefinition(): key(NULL), val(NULL) {}
  MacroDefinition(char *k, char *v): key(k), val(v) {}
  MacroDefinition(char *str) {
    // split by '='
    char *equal = strchr(str, '=');
    if (equal) {
      *equal = 0;
      key = str;
      val = equal+1;
    }
    else {
      key = str;
      val = (char*)"";
    }
  }
  char *match(const char *k) { if (!strcmp(k, key)) return val; return NULL; }
};

static std::list<MacroDefinition *> macros;

int macroDefined(const char *str, int istrue)
{
  std::list<MacroDefinition *>::iterator def;
  for (def = macros.begin(); def != macros.end(); ++def) {
    char *val = (*def)->match(str);
    if (val) {
      if (!istrue) return 1;
      else return atoi(val);
    }
  }
  return 0;
}

// input: name
// output: basename (pointer somewhere inside name)
//         scope (null if name is unscoped, newly allocated string otherwise)
void splitScopedName(const char* name, const char** scope, const char** basename) {
    const char* scopeEnd = strrchr(name, ':');
    if (!scopeEnd) {
        *scope = NULL;
        *basename = name;
        return;
    }
    *basename = scopeEnd+1;
    int len = scopeEnd-name+1; /* valid characters to copy */
    char *tmp = new char[len+1];
    strncpy(tmp, name, len);
    tmp[len]=0; /* gotta null-terminate C string */
    *scope = tmp;
}

FILE *openFile(char *interfacefile)
{
  if (interfacefile == NULL) {
    cur_file = "STDIN";
    return stdin;
  }
  else {
    cur_file=interfacefile;
    FILE *fp = fopen (interfacefile, "r") ;
    if (fp == NULL) {
      cout << "ERROR : could not open " << interfacefile << endl;
      exit(1);
    }
    return fp;
  }
  return NULL;
}

/*
ModuleList *Parse(char *interfacefile)
{
  cur_file=interfacefile;
  FILE * fp = fopen (interfacefile, "r") ;
  if (fp) {
    yyin = fp ;
    if(yyparse())
      exit(1);
    fclose(fp) ;
  } else {
    cout << "ERROR : could not open " << interfacefile << endl ;
  }
  return modlist;
}
*/

AstChildren<Module> *Parse(FILE *fp)
{
  modlist = NULL;
  yyin = fp ;
  if(yyparse())
      exit(1);
  fclose(fp) ;
  return modlist;
}

int count_tokens(FILE* fp)
{
    yyin = fp;
    int count = 0;
    while (yylex()) count++;
    return count;
}

void abortxi(char *name)
{
  cout << "Usage : " << name << " [-ansi|-f90|-intrinsic|-M]  module.ci" << endl;
  exit(1) ;
}

}

using namespace xi;

int main(int argc, char *argv[])
{
  char *fname=NULL;
  char *origFile=NULL;
  fortranMode = 0;
  internalMode = 0;
  bool dependsMode = false;
  bool countTokens = false;
  bool chareNames = false;

  for (int i=1; i<argc; i++) {
    if (*argv[i]=='-') {
      if (strcmp(argv[i],"-ansi")==0);
      else if (strcmp(argv[i],"-f90")==0)  fortranMode = 1;
      else if (strcmp(argv[i],"-intrinsic")==0)  internalMode = 1;
      else if (strncmp(argv[i],"-D", 2)==0)  macros.push_back(new MacroDefinition(argv[i]+2));
      else if (strncmp(argv[i], "-M", 2)==0) dependsMode = true;
      else if (strcmp(argv[i], "-count-tokens")==0) countTokens = true;
      else if (strcmp(argv[i], "-chare-names")==0) chareNames = true;
      else if (strcmp(argv[i], "-orig-file")==0) origFile = argv[++i];
      else abortxi(argv[0]);
    }
    else
      fname = argv[i];
  }
  //if (fname==NULL) abortxi(argv[0]);

  if (countTokens) {
      cout << count_tokens(openFile(fname)) << endl;
      return 0;
  }

  AstChildren<Module> *m = Parse(openFile(fname)) ;
  if (!m) return 0;
  m->preprocess();
  m->check();

  if (chareNames) {
    m->printChareNames();
    return 0;
  }

  if (dependsMode)
  {
      std::string ciFileBaseName;
      if (fname != NULL) {
        ciFileBaseName = fname;
      } else if (origFile != NULL) {
        ciFileBaseName = origFile;
      } else {
        abortxi(argv[0]);
      }
      size_t loc = ciFileBaseName.rfind('/');
      if(loc != std::string::npos)
          ciFileBaseName = ciFileBaseName.substr(loc+1);
      m->recurse(ciFileBaseName.c_str(), &Module::genDepend);
  }
  else
    m->recursev(&Module::generate);
  return 0 ;
}
