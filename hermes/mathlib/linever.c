void drawdel (int, int, int, int);
void drawset (int, int, int, int);
void drawinv (int, int, int, int);

void 
draw (int x1, int y1, int x2, int y2, int col)
{
  switch (col) {
    case 0:drawset (x1, y1, x2, y2);
    break;
  case 1:
    drawdel (x1, y1, x2, y2);
    break;
  case 2:
    drawinv (x1, y1, x2, y2);
    break;
  }
