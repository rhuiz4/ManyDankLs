/*========== my_main.c ==========

  This is the only file you need to modify in order
  to get a working mdl project (for now).

  my_main.c will serve as the interpreter for mdl.
  When an mdl script goes through a lexer and parser,
  the resulting operations will be in the array op[].

  Your job is to go through each entry in op and perform
  the required action from the list below:

  push: push a new origin matrix onto the origin stack

  pop: remove the top matrix on the origin stack

  move/scale/rotate: create a transformation matrix
                     based on the provided values, then
                     multiply the current top of the
                     origins stack by it.

  box/sphere/torus: create a solid object based on the
                    provided values. Store that in a
                    temporary matrix, multiply it by the
                    current top of the origins stack, then
                    call draw_polygons.

  line: create a line based on the provided values. Store
        that in a temporary matrix, multiply it by the
        current top of the origins stack, then call draw_lines.

  save: call save_extension with the provided filename

  display: view the screen
  =========================*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "parser.h"
#include "symtab.h"
#include "y.tab.h"

#include "matrix.h"
#include "ml6.h"
#include "display.h"
#include "draw.h"
#include "stack.h"
#include "gmath.h"

void my_main() {

  int i;
  struct matrix *tmp;
  struct stack *systems;
  screen s;
  zbuffer zb;
  color c;
  double step = 20;
  double theta;

  //Lighting values here for easy access
  color ambient;
  double light[2][3];
  double view[3];
  double areflect[3];
  double dreflect[3];
  double sreflect[3];

  ambient.red = 50;
  ambient.green = 50;
  ambient.blue = 50;

  light[LOCATION][0] = 0.5;
  light[LOCATION][1] = 0.75;
  light[LOCATION][2] = 1;

  light[COLOR][RED] = 0;
  light[COLOR][GREEN] = 255;
  light[COLOR][BLUE] = 255;

  view[0] = 0;
  view[1] = 0;
  view[2] = 1;

  areflect[RED] = 0.1;
  areflect[GREEN] = 0.1;
  areflect[BLUE] = 0.1;

  dreflect[RED] = 0.5;
  dreflect[GREEN] = 0.5;
  dreflect[BLUE] = 0.5;

  sreflect[RED] = 0.5;
  sreflect[GREEN] = 0.5;
  sreflect[BLUE] = 0.5;

  systems = new_stack();
  tmp = new_matrix(4, 1000);
  clear_screen( s );
  clear_zbuffer(zb);
  c.red = 0;
  c.green = 0;
  c.blue = 0;
    

  for (i = 0; i < lastop; i++){
    if (op[i].opcode == PUSH){
      //printf("PUSH\b");
      push(systems);
    }//end of push
    
    else if ( op[i].opcode == POP){
      //printf("PUSH\b");
      pop(systems);
    }//end of push
    
    else if ( op[i].opcode == BOX){
      //fgets(line, sizeof(line), f);
      //printf("BOX\t%s", line);
      /*
      sscanf(line, "%lf %lf %lf %lf %lf %lf",
	     xvals, yvals, zvals,
	     xvals+1, yvals+1, zvals+1);
      add_box(polygons, xvals[0], yvals[0], zvals[0],
                    xvals[1], yvals[1], zvals[1]);
      */
      add_box(tmp, op[i].op.box.d0[0],op[i].op.box.d0[1], op[i].op.box.d0[2], op[i].op.box.d1[0],op[i].op.box.d1[1], op[i].op.box.d1[2]);

      matrix_mult(peek(systems), tmp);
      draw_polygons(tmp, s, zb,
		    view, light, ambient, areflect, dreflect, sreflect);
      tmp->lastcol = 0;
    }//end of box
    
    else if ( op[i].opcode == SPHERE){
      //fgets(line, sizeof(line), f);
      //printf("SPHERE\t%s", line);
      /*
      sscanf(line, "%lf %lf %lf %lf",
	     xvals, yvals, zvals, &r);
      add_sphere( polygons, xvals[0], yvals[0], zvals[0], r, step_3d);
      */
      add_sphere(tmp, op[i].op.sphere.d[0], op[i].op.sphere.d[1], op[i].op.sphere.d[2], op[i].op.sphere.r, step);
      matrix_mult(peek(systems), tmp);
      draw_polygons(tmp, s, zb,
		    view, light, ambient, areflect, dreflect, sreflect);
      tmp->lastcol = 0;
    }//end of sphere
    
    else if ( op[i].opcode == TORUS){
      //fgets(line, sizeof(line), f);
      //printf("torus\t%s", line);
      /*
      sscanf(line, "%lf %lf %lf %lf %lf",
	     xvals, yvals, zvals, &r, &r1);
      add_torus(polygons, xvals[0], yvals[0], zvals[0], r, r1, step_3d);
      */
      add_torus(tmp, op[i].op.torus.d[0], op[i].op.torus.d[1], op[i].op.torus.d[2], op[i].op.torus.r0,op[i].op.torus.r1, step);

      matrix_mult(peek(systems), tmp);
      draw_polygons(tmp, s, zb,
		    view, light, ambient, areflect, dreflect, sreflect);
      tmp->lastcol = 0;
    }//end of torus
        
    else if ( op[i].opcode == LINE){
      //fgets(line, sizeof(line), f);
      //printf("LINE\t%s", line);
      /*
      sscanf(line, "%lf %lf %lf %lf %lf %lf",
	     xvals, yvals, zvals,
	     xvals+1, yvals+1, zvals+1);
      printf("%lf %lf %lf %lf %lf %lf",
	xvals[0], yvals[0], zvals[0],
             xvals[1], yvals[1], zvals[1]) 
      add_edge(edges, xvals[0], yvals[0], zvals[0],
	       xvals[1], yvals[1], zvals[1]);
      */
      add_edge(tmp, op[i].op.line.p0[0],op[i].op.line.p0[1], op[i].op.line.p0[1], op[i].op.line.p1[0],op[i].op.line.p1[1], op[i].op.line.p1[1]);
      
      matrix_mult(peek(systems), tmp);
      draw_lines(tmp, s, zb, c);
      tmp->lastcol = 0;
    }//end line
    
    else if ( op[i].opcode == SCALE){
      //fgets(line, sizeof(line), f);
      //printf("SCALE\t%s", line);
      //sscanf(line, "%lf %lf %lf",
      //     xvals, yvals, zvals);
      /* printf("%lf %lf %lf\n", */
      /*     xvals[0], yvals[0], zvals[0]); */
      //tmp = make_scale( xvals[0], yvals[0], zvals[0]);
      tmp = make_scale(op[i].op.scale.d[0], op[i].op.scale.d[1], op[i].op.scale.d[2]);

      matrix_mult(peek(systems), tmp);
      copy_matrix(tmp, peek(systems));
      tmp->lastcol = 0;
    }//end scale
    
    else if ( op[i].opcode == MOVE){
      //fgets(line, sizeof(line), f);
      //printf("MOVE\t%s", line);
      //sscanf(line, "%lf %lf %lf",
      //     xvals, yvals, zvals);
      /* printf("%lf %lf %lf\n", */
      /* xvals[0], yvals[0], zvals[0]); */
      //tmp = make_translate( xvals[0], yvals[0], zvals[0]);
      tmp = make_translate(op[i].op.move.d[0], op[i].op.move.d[1], op[i].op.move.d[2]);
      
      matrix_mult(peek(systems), tmp);
      copy_matrix(tmp, peek(systems));
      tmp->lastcol = 0;
    }//end translate
    
    else if ( op[i].opcode == ROTATE){
      //fgets(line, sizeof(line), f);
      //printf("Rotate\t%s", line);
      //sscanf(line, "%c %lf",
      //      	     &axis, &theta);
      /* printf("%c %lf\n", */
      /* axis, theta); */
      theta = op[i].op.rotate.degrees * (M_PI / 180);
      /*if ( axis == 'x' )
	tmp = make_rotX( theta );
      else if ( axis == 'y' )
	tmp = make_rotY( theta );
      else
	tmp = make_rotZ( theta );
      */
      if (op[i].op.rotate.axis == 0) //x axis
	tmp = make_rotX( theta ); 
      else if (op[i].op.rotate.axis == 1) //y axis
	tmp = make_rotY( theta );
      else //z axis
	tmp = make_rotZ( theta );
      matrix_mult(peek(systems), tmp);
      copy_matrix(tmp, peek(systems));
      tmp->lastcol = 0;
    }//end rotate
    
    else if ( op[i].opcode == DISPLAY){
      //printf("DISPLAY\t%s", line);
      display( s );
    }//end display
    
    else if ( op[i].opcode == SAVE){
      //fgets(line, sizeof(line), f);
      //*strchr(line, '\n') = 0;
      //printf("SAVE\t%s\n", line);
      save_extension(s, op[i].op.save.p->name);
    }//end save
  }
  //free_stack(systems);
  //free(s);
  //free(zb);
}
