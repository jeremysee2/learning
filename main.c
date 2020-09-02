#include <stdio.h>
#include <string.h>

void draw(int h)
{
  if (h == 0)
  {
    return;
  }
  else
  {
    draw(h-1);
    for (int i = 0; i < h; i++) 
    {
      printf("#");
    }
    printf("\n");
  }
}

int main() {
    draw(3);
}