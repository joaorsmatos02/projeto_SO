# Grupo SO-020
# Daniel Luís nº 56362
# João Matos nº 56292
# João Santos nº 57103

OBJ_dir = obj
BIN_dir = bin
OBJECTOS = main.o process.o memory.o restaurant.o driver.o client.o configuration.o log.o mesignal.o metime.o stats.o synchronization.o

magnaeats: $(OBJECTOS)
	gcc -Wall -g -o $(BIN_dir)/magnaeats $(addprefix $(OBJ_dir)/,$(OBJECTOS)) -lrt -lpthread

%.o: src/%.c $($@)
	gcc -Wall -g -I include -o $(OBJ_dir)/$@ -c $<

clean:
	rm $(addprefix $(OBJ_dir)/,$(OBJECTOS))
	rm $(BIN_dir)/magnaeats