# Clone 'libmcb' into lib directory or install it to system.
# `git clone https://github.com/at2er/libmcb`
# Don't forget compile it! Use `make` to get 'libmcb.a'
# If 'libmcb' is in lib directory
# then disable comment this line and comment the next line.
#LIBMCB = -lmcb -Llib/libmcb -Ilib/libmcb
LIBMCB = -lmcb
