rts: RTS.cpp
		g++ RTS.cpp -o rts

mfqs: mfqs.cpp
		g++ mfqs.cpp -o mfqs

clean: 
		rm rts mfqs
