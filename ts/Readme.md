Translation files of project.

Files are created by (assuming Qt in $PATH):

`lupdate -locations none -no-ui-lines -target-language $(LANG) ../src -ts $(APP)_$(LANG).ts`

Updated by:

`lupdate -locations none -no-ui-lines ../src -ts *.ts`
