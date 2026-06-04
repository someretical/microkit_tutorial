#include "printf.h"
#include "wordle.h"
#include <microkit.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/*
 * Here we initialise the word to "hello", but later in the tutorial
 * we will actually randomise the word the user is guessing.
 */
char word[WORD_LENGTH] = {'h', 'e', 'l', 'l', 'o'};

char *client_input_buffer;
char *serial_server_output_buffer;

bool is_character_in_word(char *word, int ch) {
  for (int i = 0; i < WORD_LENGTH; i++) {
    if (word[i] == ch) {
      return true;
    }
  }

  return false;
}

enum character_state char_to_state(int ch, char *word, uint64_t index) {
  if (ch == word[index]) {
    return CORRECT_PLACEMENT;
  } else if (is_character_in_word(word, ch)) {
    return INCORRECT_PLACEMENT;
  } else {
    return INCORRECT;
  }
}

void init(void) { microkit_dbg_puts("WORDLE SERVER: starting\n"); }

void notified(microkit_channel channel) {}

microkit_msginfo protected(microkit_channel channel, microkit_msginfo msginfo) {
  switch (channel) {
  case 4: {
    for (int i = 0; i < WORD_LENGTH; i++) {
      char ch = microkit_mr_get(i);
      enum character_state state = char_to_state(ch, word, i);
      microkit_mr_set(i, state);
    }
    return microkit_msginfo_new(0, WORD_LENGTH);
  }
  case 2: {
    for (int i = 0; i < WORD_LENGTH; i++) {
      word[i] = microkit_mr_get(i);
    }
    break;
  }
  }

  return microkit_msginfo_new(0, 0);
}
