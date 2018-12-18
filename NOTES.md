# Unsupported PCRE constructs

* `\0dd` `\ddd` `\0{dd...}` octal numbers
* `\Q...\E` quoting
* `\cx` control characters
* `\C` data unit
* `\h` `\H` horizontal character classes
* `\v` `\V` vertical character classes
* `\p{xx}` `\P{xx}` character properties
* `\X` unicode grapheme cluster
* boundaries other than `^$`
* atomic groups
* comments
* options/modes
* subroutines
* conditional patterns
* callouts
* match point reset `\K`


# Other unsupported "things"
* `[[.hyphen.]]` named characters
* `[[=M=]]` whatever this is

