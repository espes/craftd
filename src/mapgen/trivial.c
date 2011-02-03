/**
 * A trivial map generator for minecraft
 *
 * This map generator (for testing purposes)
 * will create an infinite plane.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "mapgen.h"
#include "../nbt/nbt.h"
#include "../algos/arith.h"

static unsigned char _blocks[32768] = {0};
static unsigned char _data[16384] = {0};
static unsigned char _skylight[16384] = {0};
static unsigned char _blocklight[16384] = {0};
static unsigned char _heightmap[256] = {0};

static void _init_data()
{
	int x, z;

	/* this should only put 1 layer of bedrock */
	for (x = 0 ; x < 16 ; x++) {
		for (z = 0 ; z < 16 ; z++) {
			_blocks[(z*128) + (x*128*16)] = 7; /* one layer bedrock */
			_skylight[(z*128) + (x*128*16)/2] = 255; /* full light for first 2 layers */
			_heightmap[z+(x*16)] = 1; /* max height is 1 */
		}
	}
}

static void gen_chunk(struct mapgen* mg, int x, int z)
{
	char x_file[8];
	char z_file[8];
	char x_dir[8];
	char z_dir[8];
	char filename[24]; /* c.<x>.<z>.dat = 24 chars*/
	char *full_path = calloc(strlen(mg->path)+(8*2)+(8*2)+8, sizeof(char));
	nbt_file *nbt;
	nbt_tag *level_tag;
	nbt_tag *sub_tag;

	itoa(x, x_file, 36);
	itoa(z, z_file, 36);
	itoa(Arith_mod(x, 64), x_dir, 36);
	itoa(Arith_mod(z, 64), z_dir, 36);

	/* test first directory exists or create it */
	strcat(full_path, mg->path);
	/* test if x directory exists or create it */
	strcat(full_path, "/");
	strcat(full_path, x_dir);
	/* test if z directory exists or create it */
	strcat(full_path, "/");
	strcat(full_path, z_dir);
	/* test if file exists */
	strcat(full_path, "/");
	snprintf(filename, 24, "c.%s.%s.dat", x_file, z_file);
	strcat(full_path, filename);

	/* generate the chunk */
	nbt_init(&nbt);

	// Level {
	nbt_new_compound(&nbt->root, "");
	nbt_new_compound(&level_tag, "Level");
	nbt_add_tag(level_tag, nbt->root);
	// Blocks
	nbt_new_byte_array(&sub_tag, "Blocks");
	nbt_set_byte_array(sub_tag, _blocks, 32768);
	nbt_add_tag(sub_tag, level_tag);
	// Data
	nbt_new_byte_array(&sub_tag, "Data");
	nbt_set_byte_array(sub_tag, _data, 16384);
	nbt_add_tag(sub_tag, level_tag);
	// SkyLight
	nbt_new_byte_array(&sub_tag, "SkyLight");
	nbt_set_byte_array(sub_tag, _skylight, 16384);
	nbt_add_tag(sub_tag, level_tag);
	// BlockLight
	nbt_new_byte_array(&sub_tag, "BlockLight");
	nbt_set_byte_array(sub_tag, _blocklight, 16384);
	nbt_add_tag(sub_tag, level_tag);
	// HeigtMap
	nbt_new_byte_array(&sub_tag, "HeightMap");
	nbt_set_byte_array(sub_tag, _heightmap, 256);
	nbt_add_tag(sub_tag, level_tag);
	// Entities
	nbt_new_list(&sub_tag, "Entities", TAG_COMPOUND);
	nbt_add_tag(sub_tag, level_tag);
	// TileEntities
	nbt_new_list(&sub_tag, "TileEntities", TAG_COMPOUND);
	nbt_add_tag(sub_tag, level_tag);
	// LastUpdate
	nbt_new_long(&sub_tag, "LastUpdate");
	nbt_set_long(sub_tag, 0);
	nbt_add_tag(sub_tag, level_tag);
	// xPos
	nbt_new_int(&sub_tag, "xPos");
	nbt_set_int(sub_tag, x);
	nbt_add_tag(sub_tag, level_tag);
	// zPos
	nbt_new_int(&sub_tag, "zPos");
	nbt_set_int(sub_tag, z);
	nbt_add_tag(sub_tag, level_tag);
	// TerrainPopulated
	nbt_new_byte(&sub_tag, "TerrainPopulated");
	nbt_set_byte(sub_tag, 0);
	nbt_add_tag(sub_tag, level_tag);
	// }

	/* write the nbt to disk */
	//nbt_write_compound(nbt, nbt_cast_compound(level_tag));
	printf("Full path : %s\n", full_path);
	nbt_write(nbt, full_path);

	free(full_path);
}

struct mapgen *init_mapgen(char *seed, char *path)
{
	struct mapgen *mg;
	if (!seed) {
	}
	if (!path) {
	}
	
	mg = calloc(1, sizeof(struct mapgen));
	
	mg->seed = strdup(seed);
	mg->path = strdup(path);
	mg->generate_chunk = &gen_chunk;
	/* we use static data here */
	_init_data();

	return mg;
}

#ifdef _MG_STANDALONE

int main()
{
	struct mapgen *mg = init_mapgen("", "./");
	mg->generate_chunk(mg, 0, 0);
}

#endif
