# User Input and UI Layout

## Controls

W, A, S and D will be used to move the cursor and the arrow keys will be used
to move the map. If the cursor is beyond the scope of the current viewport the
edge of the screen will be highlighted to indicate what direction the cursor is
in.

### Codes

#### WASD

W, A, S and D are transmitted as simple lowercase w, a, s and d. If modifier
keys such as shift or control are pressed, these will be ignored.

#### Arrows

Arrows are transmitted as control sequences.

Up:    CCI 0x41
Down:  CCI 0x42
Right: CCI 0x43
Left:  CCI 0x44
