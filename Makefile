mex/%.mex: source/%.cpp
	octave-cli -qf --eval "mex -output $@ $<"

SRCS = $(wildcard source/*.cpp)
MEXS = ${SRCS:source%.cpp=mex%.mex}

all: $(MEXS)
clean:
	rm -f $(MEXS)
