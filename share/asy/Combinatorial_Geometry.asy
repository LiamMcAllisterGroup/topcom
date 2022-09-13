// Combinatorial Geometry for asymptote
// Jörg Rambau ©2008
// modified for paper on visibility functions: Jörg Rambau ©2019

import settings;

settings.outformat = "pdf";

import three;
import graph;
import graph3;
import bsp;

// settings.prc=false;
// settings.render=16;
settings.twosided=true;


usepackage("amsmath");
usepackage("amsfonts");
// usepackage("ulsy");
usepackage("color");
usepackage("asycolors");
usepackage("mathdesign", "charter");
usepackage("inputenc","utf8");

// global variables that can be set to change the overall appearance:

real default_normal_length               = 1cm;
real default_point_size                  = 12pt;

real default_point_labelfontsize         = default_point_size / sqrt(2);
pen  default_point_labelfont_pen         = fontsize(default_point_labelfontsize);
real default_point_linewidth             = 1pt;
pen  default_point_linewidth_pen         = linewidth(default_point_linewidth);
pen  default_point_linecolor_pen         = heavyred;
pen  default_point_opacity_pen           = opacity(opacity=1, blend="Normal");
pen  default_point_fillcolor_pen         = palered;
pen  default_point_fillopacity_pen       = opacity(opacity=1, blend="Normal");
pen  default_point_overwrite_pen         = overwrite(Allow);

real default_vector_size                  = 12pt;

pen  default_point_pen                   = default_point_overwrite_pen
                                         + default_point_linewidth_pen 
                                         + default_point_linecolor_pen 
                                         + default_point_labelfont_pen 
                                         + default_point_opacity_pen;
pen  default_point_fillpen               = default_point_fillcolor_pen 
                                         + default_point_fillopacity_pen;

real default_vector_labelfontsize         = default_vector_size / sqrt(2);
pen  default_vector_labelfont_pen         = fontsize(default_vector_labelfontsize);
real default_vector_linewidth             = 1pt;
pen  default_vector_linewidth_pen         = linewidth(default_vector_linewidth);
pen  default_vector_linecolor_pen         = grey;
pen  default_vector_opacity_pen           = opacity(opacity=1, blend="Normal");
pen  default_vector_overwrite_pen         = overwrite(Allow);

pen  default_vector_pen                   = default_vector_overwrite_pen
                                         + default_vector_linewidth_pen 
                                         + default_vector_linecolor_pen 
                                         + default_vector_labelfont_pen 
                                         + default_vector_opacity_pen;

real default_pointconf_labelfontsize     = 2 * default_point_labelfontsize;
pen  default_pointconf_labelfont_pen     = fontsize(default_pointconf_labelfontsize);
real default_pointconf_linewidth         = 1pt;
pen  default_pointconf_linewidth_pen     = linewidth(default_pointconf_linewidth);
pen  default_pointconf_linecolor_pen     = blue;
pen  default_pointconf_opacity_pen       = opacity(opacity=1, blend="Normal");
pen  default_pointconf_fillcolor_pen     = paleblue;
pen  default_pointconf_fillopacity_pen   = opacity(opacity=0.5, blend="Multiply");
pen  default_pointconf_overwrite_pen     = overwrite(Move);

pen  default_pointconf_pen               = default_pointconf_overwrite_pen 
                                         + default_pointconf_linewidth_pen
                                         + default_pointconf_linecolor_pen 
                                         + default_pointconf_labelfont_pen 
                                         + default_pointconf_opacity_pen;
pen  default_pointconf_fillpen           = default_pointconf_fillcolor_pen 
                                         + default_pointconf_fillopacity_pen;

real secondary_point_size                = 18pt;

real secondary_point_labelfontsize       = secondary_point_size / sqrt(2);
pen  secondary_point_labelfont_pen       = fontsize(secondary_point_labelfontsize);
real secondary_point_linewidth           = 1.4pt;
pen  secondary_point_linewidth_pen       = linewidth(secondary_point_linewidth);
pen  secondary_point_linecolor_pen       = red;
pen  secondary_point_opacity_pen         = opacity(opacity=1, blend="Normal");
pen  secondary_point_fillcolor_pen       = yellow;
pen  secondary_point_fillopacity_pen     = opacity(opacity=1, blend="Normal");
pen  secondary_point_overwrite_pen       = overwrite(Allow);

real secondary_pointconf_labelfontsize   = 2 * default_point_labelfontsize;
pen  secondary_pointconf_labelfont_pen   = fontsize(secondary_pointconf_labelfontsize);
real secondary_pointconf_linewidth       = 1pt;
pen  secondary_pointconf_linewidth_pen   = linewidth(secondary_pointconf_linewidth);
pen  secondary_pointconf_linecolor_pen   = heavygreen;
pen  secondary_pointconf_opacity_pen     = opacity(opacity=1, blend="Normal");
pen  secondary_pointconf_fillcolor_pen   = palegreen;
pen  secondary_pointconf_fillopacity_pen = opacity(opacity=0.5, blend="Multiply");
pen  secondary_pointconf_overwrite_pen   = overwrite(Move);

pen  secondary_point_pen                 = secondary_point_overwrite_pen
                                         + secondary_point_linewidth_pen 
                                         + secondary_point_linecolor_pen 
                                         + secondary_point_labelfont_pen 
                                         + secondary_point_opacity_pen;
pen  secondary_point_fillpen             = secondary_point_fillcolor_pen 
                                         + secondary_point_fillopacity_pen;
pen  secondary_pointconf_pen             = secondary_pointconf_overwrite_pen 
                                         + secondary_pointconf_linewidth_pen
                                         + secondary_pointconf_linecolor_pen 
                                         + secondary_pointconf_labelfont_pen 
                                         + secondary_pointconf_opacity_pen;
pen  secondary_pointconf_fillpen         = secondary_pointconf_fillcolor_pen 
                                         + secondary_pointconf_fillopacity_pen;

real default_point3_size                  = 12pt;

real default_point3_labelfontsize         = default_point3_size / sqrt(2);
pen  default_point3_labelfont_pen         = fontsize(default_point3_labelfontsize);
real default_point3_linewidth             = default_point3_size / 10;
pen  default_point3_linewidth_pen         = linewidth(default_point3_linewidth);
pen  default_point3_linecolor_pen         = heavyred;
pen  default_point3_opacity_pen           = opacity(opacity=1, blend="Normal");
pen  default_point3_fillcolor_pen         = palered;
pen  default_point3_fillopacity_pen       = opacity(opacity=1, blend="Normal");
pen  default_point3_overwrite_pen         = overwrite(Allow);

real default_pointconf3_labelfontsize     = 2 * default_point3_labelfontsize;
pen  default_pointconf3_labelfont_pen     = fontsize(default_pointconf3_labelfontsize);
real default_pointconf3_linewidth         = 1pt;
pen  default_pointconf3_linewidth_pen     = linewidth(default_pointconf3_linewidth);
pen  default_pointconf3_linecolor_pen     = blue;
pen  default_pointconf3_opacity_pen       = opacity(opacity=1, blend="Normal");
pen  default_pointconf3_fillcolor_pen     = paleblue;
pen  default_pointconf3_fillopacity_pen   = opacity(opacity=0.5, blend="Normal");
pen  default_pointconf3_overwrite_pen     = overwrite(Move);

pen  default_point3_pen                   = default_point3_overwrite_pen
                                         + default_point3_linewidth_pen 
                                         + default_point3_linecolor_pen 
                                         + default_point3_labelfont_pen 
                                         + default_point3_opacity_pen;
pen  default_point3_fillpen               = default_point3_fillcolor_pen 
                                         + default_point3_fillopacity_pen;
pen  default_pointconf3_pen               = default_pointconf3_overwrite_pen 
                                         + default_pointconf3_linewidth_pen
                                         + default_pointconf3_linecolor_pen 
                                         + default_pointconf3_labelfont_pen 
                                         + default_pointconf3_opacity_pen;
pen  default_pointconf3_fillpen           = default_pointconf3_fillcolor_pen 
                                         + default_pointconf3_fillopacity_pen;

real secondary_point3_size                = 18pt;

real secondary_point3_labelfontsize       = secondary_point3_size / sqrt(2);
pen  secondary_point3_labelfont_pen       = fontsize(secondary_point3_labelfontsize);
real secondary_point3_linewidth           = secondary_point3_size / 10;
pen  secondary_point3_linewidth_pen       = linewidth(secondary_point3_linewidth);
pen  secondary_point3_linecolor_pen       = red;
pen  secondary_point3_opacity_pen         = opacity(opacity=1, blend="Normal");
pen  secondary_point3_fillcolor_pen       = yellow;
pen  secondary_point3_fillopacity_pen     = opacity(opacity=1, blend="Normal");
pen  secondary_point3_overwrite_pen       = overwrite(Allow);

real secondary_pointconf3_labelfontsize   = 2 * default_point3_labelfontsize;
pen  secondary_pointconf3_labelfont_pen   = fontsize(secondary_pointconf3_labelfontsize);
real secondary_pointconf3_linewidth       = 2pt;
pen  secondary_pointconf3_linewidth_pen   = linewidth(secondary_pointconf3_linewidth);
pen  secondary_pointconf3_linecolor_pen   = heavygreen;
pen  secondary_pointconf3_opacity_pen     = opacity(opacity=1, blend="Normal");
pen  secondary_pointconf3_fillcolor_pen   = palegreen;
pen  secondary_pointconf3_fillopacity_pen = opacity(opacity=0.5, blend="Normal");
pen  secondary_pointconf3_overwrite_pen   = overwrite(Move);

pen  secondary_point3_pen                 = secondary_point3_overwrite_pen
                                         + secondary_point3_linewidth_pen 
                                         + secondary_point3_linecolor_pen 
                                         + secondary_point3_labelfont_pen 
                                         + secondary_point3_opacity_pen;
pen  secondary_point3_fillpen             = secondary_point3_fillcolor_pen 
                                         + secondary_point3_fillopacity_pen;
pen  secondary_pointconf3_pen             = secondary_pointconf3_overwrite_pen 
                                         + secondary_pointconf3_linewidth_pen
                                         + secondary_pointconf3_linecolor_pen 
                                         + secondary_pointconf3_labelfont_pen 
                                         + secondary_pointconf3_opacity_pen;
pen  secondary_pointconf3_fillpen         = secondary_pointconf3_fillcolor_pen 
                                         + secondary_pointconf3_fillopacity_pen;

// some color enhancements:

projection get_currentprojection() {
  return currentprojection;
}

light get_currentlight() {
  return currentlight;
}

pen lightcolor(path3 p3, pen p) {
   return color(normal(p3),
 	       material(diffusepen=p,
 			//ambientpen=black,
 			emissivepen=0.2*p
 			//specularpen=mediumgrey
			),
 	       get_currentlight());
//   return color(normal(p3),
// 	       p,
// 	       get_currentlight());
}
// three enhancements:

// implement mylook: transform3 that shows reference triple "target"
// as orthogonally seen from triple "eye", where "up" is considered the 3D-up-direction,
// i.e., the image of Y is up projected to the plane orthogonal to eye minus target:
transform3 mylook(triple eye, triple up=Z, triple target=O) {
  triple planenormal = eye - target;
  if (planenormal == O) {
    // write("eye and target identical: choosing Z as normal direction");
    planenormal = Z;
  }
  //  write("planenormal = ", planenormal);
  // image of Y (standard upwards direction of untransformed labels):
  triple updir = planeproject(planenormal) * up;
  // if up happens to be parallel to planenormal, try all coordinate directions:
  if ((updir == O) && (up != Z)) {
    //  write("up parallel to eye minus target: trying Z as up direction");
    updir = planeproject(planenormal) * Z;
  }
  if ((updir == O) && (up != Y)) {
    //  write("up parallel to eye minus target: trying Y as up direction");
    updir = planeproject(planenormal) * Y;
  }
  if ((updir == O) && (up != X)) {
    //  write("up parallel to eye minus target: trying X as up direction");
    updir = planeproject(planenormal) * X;
  }
  // normalize:
  updir = unit(updir);
  // write("updir = ", updir);
  // image of X (standard writing direction of untransformed labels)
  triple rightdir = unit(cross(updir, planenormal));
  // write("rightdir = ", rightdir);
  // build the unique special orthogonal transform without translation
  // that mapx X to rightdir and Y to updir and Z to planenormal:
  transform3 result = new real[][]
                      {{rightdir.x, updir.x, planenormal.x, 0},
                       {rightdir.y, updir.y, planenormal.y, 0},
	               {rightdir.z, updir.z, planenormal.z, 0},
		       {0,          0,       0,             1}};
  // add the shift of target:
  return shift(target) * result;
}

pair ORIGIN = (0,0);
pair UP     = (0,1);
pair RIGHT  = (1,0);

triple default_point_normal() {
  return invert((0,0), Z, Z * cm, get_currentprojection());
}
triple default_label3_normal() {
  return invert((0,0), Z, Z * cm, get_currentprojection());
}
triple default_label3_position() {
  return invert((0,0), Z, Z * cm, get_currentprojection());
}

void label(picture pic=currentpicture, 
	   Label L, 
	   triple global_up=Z,
	   triple label3_position=default_label3_position(),
	   triple label3_normal=default_point_normal(),
	   transform label_transform=identity(),
           align label_align=L.align,
	   pen p=nullpen,
	   projection P=get_currentprojection()) {
  label(pic, 
 	mylook(eye=label3_normal, target=O, up=global_up)*(label_transform*scale(abs(P.vector())/abs(P.camera-label3_position))*L),
 	position=label3_position+0.01pt*unit(label3_normal), 
 	align=label_align,
 	p=p,
 	Embedded);
}

void label(face[] facelist,
	   Label L, 
	   triple global_up=Z,
	   triple label3_position=default_label3_position(),
	   triple label3_normal=default_point_normal(),
	   transform label_transform=identity(),
           align label_align=L.align,
	   pen p=nullpen,
	   projection P=get_currentprojection()) {
  path[] parray = texpath(label_transform*scale(abs(P.vector())/abs(P.camera-label3_position))*L);
  while (parray.length > 0) {
    path3 p3 = shift(label3_position+0.1*unit(label3_normal))*mylook(eye=label3_normal, target=O, up=global_up)*path3(parray.pop());
    filldraw(facelist.push(p3), project(p3), drawpen=nullpen, fillpen=p);
  }
    //  path3 Lpath = mylook(eye=label3_normal, target=O, up=global_up)*(label_transform*scale(abs(P.vector())/abs(P.camera-label3_position))*L);
//   label(facelist.push(new face),
// 	L,
// 	position=label3_position, 
// 	align=label_align,
// 	p=p,
// 	Embedded);
}
// sometimes, straight faces are easier drawn as fake non-straight paths:
path3 fake_nonstraight_path(path3 p3,
			    pen pointconf_pen = default_pointconf3_pen) {
  triple start = point(p3, 0.0);
  triple end = point(p3, 1.0);
  triple normal = default_point_normal();
  triple dir2d = 0.1*linewidth(pointconf_pen) * unit(cross(end - start, normal));
  p3 = shift(-0.5 * dir2d) * p3;
  p3 = p3--(end + dir2d)--(start + dir2d)--cycle;
  if (normal(p3) == O) {
    write(start);
    write(end);
    write(p3);
    abort("path not 2d – aborting.");
  }
  return p3;
}

// a structure for sorting points in counter-clockwise order:
struct index_angle_pair {
  int  index;
  real angle;

  void operator init(int new_index, pair new_coord) {
    this.index = new_index;
    this.angle = angle(new_coord);
  }
}

bool index_angle_pair_less(index_angle_pair iap1, index_angle_pair iap2) {
  return (iap1.angle < iap2.angle);
}

struct labeled_point {
  Label point_label = "";
  pair  point_coord;

  void operator init(labeled_point p) {
    this.point_label = p.point_label;
    this.point_coord = p.point_coord;
  }

  void operator init(Label L="", pair coord) {
    this.point_label = L;
    this.point_coord = coord;
  }

  void draw_point(picture pic = currentpicture, 
		  real point_size=default_point_size, 
		  pen point_pen=default_point_pen, 
		  pen point_fillpen=default_point_fillpen) {
    path p = circle(this.point_coord, point_size/2)--cycle;
    draw(pic, p, p=point_pen, align=Center);
    fill(pic, p, p=point_fillpen);
  }
  void draw_vector(picture pic = currentpicture, 
		   pen vector_pen=default_vector_pen) {
    path p = (0,0)--this.point_coord;
    draw(pic, p, p=vector_pen, align=Center, arrow=Arrow);
  }
  
  void draw_unitvector(picture pic = currentpicture,
		       real radius,
		       pen vector_pen=default_vector_pen) {
    path p = (0,0)--radius * unit(this.point_coord);
    draw(pic, p, p=vector_pen, align=Center, arrow=Arrow);
  }

  void draw_label(picture pic = currentpicture, 
		  real point_size=default_point_size, 
		  pen point_pen=default_point_pen, 
		  pen point_fillpen=default_point_fillpen) {
    label(pic, this.point_label, this.point_coord, p=point_pen, align=Center);
  }
  void draw(picture pic = currentpicture, 
	    real point_size=default_point_size, 
	    pen point_pen=default_point_pen, 
	    pen point_fillpen=default_point_fillpen) {
    this.draw_point(pic, point_size, point_pen, point_fillpen);
    this.draw_label(pic, point_size, point_pen, point_fillpen);
  }
    
};

void draw(picture pic=currentpicture, 
	  labeled_point lp,
	  real point_size=default_point_size, 
	  pen point_pen=default_point_pen, 
	  pen point_fillpen=default_point_fillpen) {
  lp.draw(pic, point_size, point_pen, point_fillpen);
}

struct pointconf {
  Label           pointconf_label = "";
  labeled_point[] pointconf_allpoints;
  int[]           pointconf_cell;
  int[]           pointconf_extremepoints;

  void operator init(Label L = "") {
    this.pointconf_allpoints     = new labeled_point[] {};
    this.pointconf_label         = L;
    this.pointconf_cell          = new int[] {};
    this.pointconf_extremepoints = new int[] {};
  }

  void operator init(Label           L             = "",
		     labeled_point[] allpoints     = new labeled_point[] {},
		     int[]           cell          = new int[] {},
		     int[]           extremepoints = new int[] {}) {
    this.pointconf_label         = L;
    this.pointconf_allpoints     = allpoints;
    this.pointconf_cell          = cell;
    this.pointconf_extremepoints = extremepoints;
  }
 		     
  
  void operator init(pointconf A,
		     Label L=A.pointconf_label,
		     int[] cell=copy(A.pointconf_cell),
		     int[] extremepoints=copy(A.pointconf_extremepoints),
		     bool copy=false) {
    if (copy) {
      for (labeled_point p : A.pointconf_allpoints) {
	this.pointconf_allpoints.push(labeled_point(p));
      }
      this.pointconf_cell          = copy(cell);
      this.pointconf_extremepoints = copy(extremepoints);
    }
    else {
      this.pointconf_allpoints     = A.pointconf_allpoints;
      this.pointconf_cell          = cell;
      this.pointconf_extremepoints = extremepoints;
    }
    this.pointconf_label         = L;
  }

  void operator init(Label L="", pair[] coords, int[] extremepoints={}, int start_label = 1) {
    this.pointconf_label = L;
    int cnt = 0;
    for (pair coord : coords) {
      labeled_point p = new labeled_point;
      p.point_label   = Label(format("$%d$", cnt+start_label), align=Center);
      p.point_coord   = coord;
      this.pointconf_allpoints.push(p);
      this.pointconf_cell.push(cnt);
      ++cnt;
    }
    for (int i : extremepoints) {
      this.pointconf_extremepoints.push(i);
    }
  }

  pair[] coords() {
    pair[] result;
    for (int i : this.pointconf_cell) {
      result.push(this.pointconf_allpoints[i].point_coord);
    }
    return result;
  }

  int add_point(labeled_point p, bool extreme=false, int extremepoints_insertion_index = this.pointconf_extremepoints.length) {
    int new_index = this.pointconf_allpoints.length;
    this.pointconf_allpoints.push(p);
    this.pointconf_cell.push(new_index);
    if (extreme) {
      this.pointconf_extremepoints.insert(extremepoints_insertion_index, new_index);
    }
    return new_index;
  }

  int add_point(pair coord, bool extreme=false, int extremepoints_insertion_index = this.pointconf_extremepoints.length) {
    int new_index = this.pointconf_allpoints.length;
    labeled_point p = new labeled_point;
    p.point_label   = Label(format("$%d$", new_index), align=Center);
    p.point_coord   = coord;
    this.add_point(p, extreme, extremepoints_insertion_index);
    return new_index;
  }

  pair average_position(int[] cell=this.pointconf_cell, 
			real[] weight=sequence(new  real(int) { return 1/cell.length; }, cell.length)) {
    pair result = 0;
    real weight_sum = 0;
    for (int i = 0; i < cell.length; ++i) {
      result = result + weight[i] * this.pointconf_allpoints[cell[i]].point_coord;
      weight_sum = weight_sum + weight[i];
    }
    return 1 / weight_sum * result;
  }
  
  pair[] intersection_points(int[] cell1 = this.pointconf_cell, int[] cell2 = this.pointconf_cell) {
    pair[] edge_intersections;
    for (int i1 = 0; i1 < cell1.length; ++i1) {
      for (int j1 = i1 + 1; j1 < cell1.length; ++j1) {
	path p1 = this.pointconf_allpoints[cell1[i1]].point_coord--this.pointconf_allpoints[cell1[j1]].point_coord;
	for (int i2 = 0; i2 < cell2.length; ++i2) {
	  for (int j2 = i2 + 1; j2 < cell2.length; ++j2) {
	    if (((cell1[i1] != cell2[i2]) || (cell1[j1] != cell2[j2])) && ((cell1[i1] != cell2[j2]) || (cell1[j1] != cell2[i2]))) {
	      //	      write("checking ", cell1[i1], cell1[j1], cell2[i2], cell2[j2]);
	      if ((cell1[i1] == cell2[i2]) || (cell1[i1] == cell2[j2])) {
		edge_intersections.push(this.pointconf_allpoints[cell1[i1]].point_coord);
	      }
	      else if ((cell1[j1] == cell2[i2]) || (cell1[j1] == cell2[j2])) {
		edge_intersections.push(this.pointconf_allpoints[cell1[j1]].point_coord);
	      }
	      else {
		path p2 = this.pointconf_allpoints[cell2[i2]].point_coord--this.pointconf_allpoints[cell2[j2]].point_coord;
		pair[] new_intersections = intersectionpoints(p1, p2);
		for (pair x : new_intersections) {
		  edge_intersections.push(x);
		}
	      }
	    }
	  }
	}
      }
    }
    return edge_intersections;
  }

  pair interior_intersection_point(int[] cell1, int[] cell2) {
    pair[] points = this.intersection_points(cell1, cell2);
    return sum(points) / points.length;
  }

  int[] add_intersection_points(int[] cell1 = this.pointconf_cell, int[] cell2 = this.pointconf_cell) {
    int[] result;
    pair[] points = this.intersection_points(cell1, cell2);
    for (pair point : points) {
      result.push(this.add_point(point));
    }
    return result;
  }

  pair facet_normal(int i1, int i2) {
    pair p1 = this.pointconf_allpoints[i1].point_coord;
    pair p2 = this.pointconf_allpoints[i2].point_coord;
    pair facet_normal = rotate(90) * unit(p2 - p1);
    return facet_normal;
  }

  path dimtwo_simplicialcone(int i1, int i2, real radius) {
    path p;
    int[] cell = this.pointconf_cell;
    pair u = radius * unit(this.pointconf_allpoints[i1].point_coord);
    pair v = radius * unit(this.pointconf_allpoints[i2].point_coord);
    p = arc((0,0), u, v, direction=CCW);
    p = p--(0,0);
    p = p--cycle;
    return p;
  }

  void draw_points(picture pic = currentpicture, 
		   int[] visiblepoints = this.pointconf_cell,
		   real point_size=default_point_size,
		   pen point_pen = default_point_pen,
		   pen point_fillpen = default_point_fillpen) {

    // draw points if requested:
    for (int i : visiblepoints) {
      labeled_point p = this.pointconf_allpoints[i];
      p.draw_point(pic, point_size=point_size, point_pen=point_pen, point_fillpen=point_fillpen);
    }
  }

  void draw_vectors(picture pic = currentpicture,
		    int[] visiblepoints = this.pointconf_cell,
		    pen vector_pen = default_vector_pen) {
    // draw vectors if requested:
    for (int i : visiblepoints) {
      labeled_point p = this.pointconf_allpoints[i];
      p.draw_vector(pic, vector_pen=vector_pen);
    }
    labeled_point origin = labeled_point((0,0));
    origin.draw(pic, point_size=default_point_size/2, point_pen=vector_pen, point_fillpen=vector_pen);
  }
  void draw_unitvectors(picture pic = currentpicture,
			real radius,
			int[] visiblepoints = this.pointconf_cell,
			pen vector_pen = default_vector_pen) {
    // draw vectors if requested:
    for (int i : visiblepoints) {
      labeled_point p = this.pointconf_allpoints[i];
      p.draw_unitvector(pic, radius, vector_pen=vector_pen);
    }
    labeled_point origin = labeled_point((0,0));
    origin.draw(pic, point_size=default_point_size/2, point_pen=vector_pen, point_fillpen=vector_pen);
  }
  
  void draw_polygon(picture pic = currentpicture, 
 		    pen pointconf_pen = default_pointconf_pen,
		    pen pointconf_fillpen = default_pointconf_fillpen) {

    index_angle_pair[] index_angle_pairs;
    pair centerpoint = this.average_position(this.pointconf_extremepoints);
    for (int i : this.pointconf_extremepoints) {
      index_angle_pairs.push(index_angle_pair(i, this.pointconf_allpoints[i].point_coord - centerpoint));
    }
    index_angle_pair[] index_angle_pairs_sorted = sort(index_angle_pairs, index_angle_pair_less);
    
    // draw polygon along given extreme points:
    if (this.pointconf_extremepoints.length > 1) {
      guide convex_hull;
      for (index_angle_pair iap : index_angle_pairs_sorted) {
	labeled_point p = this.pointconf_allpoints[iap.index];
	convex_hull = convex_hull--p.point_coord;
      }
      convex_hull = convex_hull--cycle;
      draw(pic, convex_hull, p=pointconf_pen);
      fill(pic, convex_hull, p=pointconf_fillpen);
    }
  }

  void draw_facetnormals(picture pic = currentpicture,
			 int[] firstpoints = sequence(this.pointconf_extremepoints.length),
			 pen vector_pen = default_vector_pen,
			 real normal_length = default_normal_length) {
    if (firstpoints.length > 1) {
      for (int i = 0; i < firstpoints.length; ++i) {
	int ind_i = firstpoints[i];
	int succ_i = (ind_i+1) % this.pointconf_extremepoints.length;
	int start_i = this.pointconf_extremepoints[ind_i];
	int stop_i = this.pointconf_extremepoints[succ_i];
	pair p1 = this.pointconf_allpoints[start_i].point_coord;
	pair p2 = this.pointconf_allpoints[stop_i].point_coord;
	pair normal_dir = this.facet_normal(start_i, stop_i);
	pair normal_point = (p1 + p2) / 2;
	draw(pic, normal_point--normal_point + normal_length * normal_dir, p=vector_pen, arrow=Arrow);
      }
    }
  }
  
  void draw_chambercones(picture pic = currentpicture,
			 real radius,
			 pen pointconf_pen = default_pointconf_pen,
			 pen pointconf_fillpen = default_pointconf_fillpen) {
    int[] extremepoints = this.pointconf_extremepoints;
    for (int i = 0; i < extremepoints.length; ++i) {
      path p = this.dimtwo_simplicialcone(extremepoints[i], extremepoints[(i+1) % extremepoints.length], radius);
      filldraw(pic,
	       p,
	       drawpen=pointconf_pen,
	       fillpen=pointconf_fillpen);
    }
  }

  void draw_pointconflabel(picture pic = currentpicture,
			   pair label_position = this.pointconf_label.position,
			   pen pointconf_pen = default_pointconf_pen) {
    label(pic, this.pointconf_label, position = label_position, p=pointconf_pen);
  }

  void draw_pointlabels(picture pic = currentpicture,
			real point_size = default_point3_size,
			pen point_pen = default_point3_pen,
			pen point_fillpen = default_point3_fillpen) {
    
    // draw labels of visible points:
    for (int i : this.pointconf_cell) {
      labeled_point p = this.pointconf_allpoints[i];
      p.draw_label(pic, 
		   point_size, 
		   point_pen, 
		   point_fillpen);
    }
  }
  
  void draw(picture pic = currentpicture, 
	    int[] visiblepoints = this.pointconf_cell, 
	    pair label_position = this.pointconf_label.position,
	    pen pointconf_pen = default_pointconf_pen,
	    pen pointconf_fillpen = default_pointconf_fillpen,
	    real point_size=default_point_size,
	    pen point_pen = default_point_pen,
	    pen point_fillpen = default_point_fillpen) {
    draw_polygon(pic, pointconf_pen, pointconf_fillpen);
    draw_points(pic, visiblepoints, point_size, point_pen, point_fillpen);
    draw_pointlabels(pic, point_size, point_pen, point_fillpen);
    draw_pointconflabel(pic, label_position, pointconf_pen);
  }
};

void draw(picture pic=currentpicture,
	  pointconf pc,
	  int[] visiblepoints = pc.pointconf_cell, 
	  pair label_position = pc.average_position(pc.pointconf_cell),
	  pen pointconf_pen = default_pointconf_pen,
	  pen pointconf_fillpen = default_pointconf_fillpen,
	  real point_size=default_point_size,
	  pen point_pen = default_point_pen,
	  pen point_fillpen = default_point_fillpen) {
  pc.draw(pic, visiblepoints, label_position, pointconf_pen, pointconf_fillpen, point_size, point_pen, point_fillpen);
}

struct labeled_point3 {
  Label  point3_label = "";
  triple point3_coord;

  void operator init(labeled_point3 p) {
    this.point3_label = p.point3_label;
    this.point3_coord = p.point3_coord;
  }

  void operator init(Label L="", triple coord) {
    this.point3_label = L;
    this.point3_coord = coord;
  }

  path3 point_path3(triple point_normal=default_point_normal(),
		    real point_size=default_point3_size) {
    path3 result = circle(this.point3_coord, point_size/2, normal=point_normal)--cycle;
    return result;
  }

  surface point_surface(triple point_normal=default_point_normal(),
			real point_size=default_point3_size,
			pen point_fillpen=default_point_fillpen) {
    surface result = surface(this.point_path3(point_normal, point_size), planar=true);
    return result;
  }
  
  void draw_point(picture pic = currentpicture, 
		  triple point_normal=default_point_normal(), 
		  real point_size=default_point3_size, 
		  pen point_pen=default_point3_pen, 
		  pen point_fillpen=default_point3_fillpen) {
    draw(pic, this.point_surface(point_normal, point_size), meshpen=nullpen, surfacepen=point_fillpen);
    draw(pic, this.point_path3(point_normal, point_size), p=point_pen, light=currentlight);
  }

  void draw_point(face[] facelist,
		  triple point_normal=default_point_normal(), 
		  real point_size=default_point3_size, 
		  pen point_pen=default_point3_pen, 
		  pen point_fillpen=default_point3_fillpen) {
    path3 p3 = this.point_path3(point_normal, point_size);
    //    write("drawing point path ...");
    filldraw(facelist.push(p3), project(p3), drawpen=point_pen, fillpen=point_fillpen);
    //    write("...done.");
  }

  void draw_vector(picture pic = currentpicture, 
		   pen vector_pen=default_vector_pen) {
    path3 p = O--this.point3_coord;
    draw(pic, p, p=vector_pen, align=Center, arrow=Arrow3(DefaultHead3), light=currentlight);
  }

  void draw_vector(face[] facelist,
		   pen vector_pen=default_vector_pen) {
    path3 p3 = fake_nonstraight_path(this.point3_coord--O, vector_pen);
    draw(facelist.push(p3), project(p3), p=vector_pen, arrow=Arrow);
  }

  void draw_unitvector(picture pic = currentpicture,
		       real radius,
		       pen vector_pen=default_vector_pen) {
    path3 p = O--radius * unit(this.point3_coord);
    draw(pic, p, p=vector_pen, align=Center, arrow=Arrow3(DefaultHead3), light=currentlight);
  }

  void draw_unitvector(face[] facelist,
		       real radius,
		       pen vector_pen=default_vector_pen) {
    path3 p3 = fake_nonstraight_path(this.point3_coord--O, vector_pen);
    draw(facelist.push(p3), project(p3), p=vector_pen, align=Center, arrow=Arrow);
  }

  void draw_label(picture pic = currentpicture, 
		  triple global_up=Z,
		  triple point_normal=default_point_normal(),
		  transform label_transform=identity(),
		  real point_size=default_point3_size, 
		  pen point_pen=default_point3_pen, 
		  pen point_fillpen=default_point3_fillpen) {
    label(pic=pic, 
	  L=this.point3_label, 
	  global_up=global_up,
	  label3_position=this.point3_coord, 
	  label3_normal=point_normal,
	  label_transform=label_transform,
	  label_align=Center, 
	  p=point_pen, 
	  P=get_currentprojection());
  }
  void draw_label(face[] facelist,
		  triple global_up=Z,
		  triple point_normal=default_point_normal(),
		  transform label_transform=identity(),
		  real point_size=default_point3_size, 
		  pen point_pen=default_point3_pen, 
		  pen point_fillpen=default_point3_fillpen) {
    label(facelist, 
	  L=this.point3_label, 
	  global_up=global_up,
	  label3_position=this.point3_coord, 
	  label3_normal=point_normal,
	  label_transform=label_transform,
	  label_align=Center, 
	  p=point_pen, 
	  P=get_currentprojection());
  }

  void draw(picture pic = currentpicture, 
	    triple global_up=Z,
	    triple point_normal=default_point_normal(), 
	    transform label_transform=identity(),
	    real point_size=default_point3_size, 
	    pen point_pen=default_point3_pen, 
	    pen point_fillpen=default_point3_fillpen) {
    this.draw_point(pic, point_normal, point_size, point_pen, point_fillpen);
    this.draw_label(pic, global_up, point_normal, label_transform, point_size, point_pen, point_fillpen);
  }
  void draw(face[] facelist,
	    triple global_up=Z,
	    triple point_normal=default_point_normal(), 
	    transform label_transform=identity(),
	    real point_size=default_point3_size, 
	    pen point_pen=default_point3_pen, 
	    pen point_fillpen=default_point3_fillpen) {
    this.draw_point(facelist, point_normal, point_size, point_pen, point_fillpen);
    this.draw_label(facelist, global_up, point_normal, label_transform, point_size, point_pen, point_fillpen);
  }

};

void draw(picture pic=currentpicture,
	  labeled_point3 lp3,
	  triple point_normal=default_point_normal(),
	  transform label_transform=identity(),
	  real point_size=default_point3_size,
	  pen point_pen=default_point3_pen,
	  pen point_fillpen=default_point3_fillpen) {
  lp3.draw(pic, point_normal, label_transform, point_size, point_pen, point_fillpen);
}

labeled_point project(labeled_point3 p, projection P=currentprojection) {
  labeled_point result = new labeled_point;
  result.point_label = p.point3_label;
  result.point_coord = project(p.point3_coord, P);
  return result;
}

labeled_point3 homogenize(labeled_point p, triple normal=Z, triple point=Z) {
  labeled_point3 p3;
  p3.point3_label = p.point_label;
  p3.point3_coord = invert(p.point_coord, normal=normal, point=point, orthographic(normal));
  return p3;
}


struct pointconf3 {
  Label            pointconf3_label   = "";
  labeled_point3[] pointconf3_allpoints;
  int[]            pointconf3_cell;
  int[][]          pointconf3_facets;

  
  void operator init(Label            L             = "",
		     labeled_point3[] allpoints     = new labeled_point3[] {},
		     int[]            cell          = new int[] {},
		     int[][]          facets        = new int[][] {}) {
    this.pointconf3_label         = L;
    this.pointconf3_allpoints     = allpoints;
    this.pointconf3_cell          = cell;
    this.pointconf3_facets        = facets;
  }

  void operator init(pointconf3 A, 
		     Label L=A.pointconf3_label, 
		     int[] cell=copy(A.pointconf3_cell), 
		     int[][] facets=copy(A.pointconf3_facets),
		     bool copy=false) {
    if (copy) {
      for (labeled_point3 p : A.pointconf3_allpoints) {
	this.pointconf3_allpoints.push(labeled_point3(p));
      }
      this.pointconf3_cell          = copy(cell);
      this.pointconf3_facets        = copy(facets);
    }
    else {
      this.pointconf3_allpoints     = A.pointconf3_allpoints;
      this.pointconf3_cell          = cell;
      this.pointconf3_facets        = facets;
    }
    pointconf3_label         = L;
  }

  void operator init(Label L="", triple[] coords={}, int[] cell={}, int[][] facets={}, int start_label = 1) {
    this.pointconf3_label = L;
    int cnt = 0;
    for (triple coord : coords) {
      labeled_point3 p = new labeled_point3;
      p.point3_label   = Label(format("$%d$", cnt+start_label), align=Center);
      p.point3_coord   = coord;
      this.pointconf3_allpoints.push(p);
      this.pointconf3_cell.push(cnt);
      this.pointconf3_facets = facets;
      ++cnt;
    }
  }

  int add_point(labeled_point3 p, int[][] new_facets = new int[][]{}) {
    int new_index = this.pointconf3_allpoints.length;
    this.pointconf3_allpoints.push(p);
    this.pointconf3_cell.push(new_index);
    for (int[] facet : new_facets) {
      this.pointconf3_facets.push(facet);
    }
    return new_index;
  }

  int add_point(triple coord, int[][] new_facets = new int[][]{}) {
    int new_index = this.pointconf3_allpoints.length;
    labeled_point3 p = new labeled_point3;
    p.point3_label   = Label(format("$%d$", new_index), align=Center);
    p.point3_coord   = coord;
    this.add_point(p, new_facets);
    return new_index;
  }


  triple average_position(int[] cell=this.pointconf3_cell, 
			  real[] weight=sequence(new  real(int) { return 1/cell.length; }, cell.length)) {
    triple result = O;
    real weight_sum = 0;
    for (int i = 0; i < cell.length; ++i) {
      result = result + weight[i] * this.pointconf3_allpoints[cell[i]].point3_coord;
      weight_sum = weight_sum + weight[i];
    }
    return 1 / weight_sum * result;
  }
  path3 facet_path3(int[] facet) {
    guide3 convex_hull;
    for (int i : facet) {
      labeled_point3 p = this.pointconf3_allpoints[i];
      convex_hull = convex_hull--p.point3_coord;
    }
    convex_hull = convex_hull--cycle;
    return convex_hull;
  }

  path3[] boundary_path3array() {
    path3[] result;
    for (int[] facet : this.pointconf3_facets) {
      write("adding facet to path3array ...", facet);
      result.push(this.facet_path3(facet));
      write("... done.");
    }
    return result;
  }

  surface boundary_surface() {
    surface boundary;
    for (int[] facet : this.pointconf3_facets) {
      write("adding facet to surface ...", facet);
      path3 convex_hull = this.facet_path3(facet);
      boundary.append(surface(convex_hull, planar=true));
      write("... done.");
    }
    return boundary;
  }

  triple facet_normal(int[] facet) {
    return normal(this.facet_path3(facet));
  }
      
  path3 dimtwo_simplicialcone(int i1, int i2, real radius) {
    path3 p3;
    int[] cell = this.pointconf3_cell;
    triple u = radius * unit(this.pointconf3_allpoints[i1].point3_coord);
    triple v = radius * unit(this.pointconf3_allpoints[i2].point3_coord);
    if (u == v) {
      abort("cell not two-dimensional");
    }
    triple normal = unit(cross(u, v));
    p3 = arc(O, u, v, normal, direction=CCW);
    p3 = p3--O;
    p3 = p3--cycle;
    return p3;
  }

  triple[] intersection_vectors(int[] cell1 = this.pointconf3_cell, int[] cell2 = this.pointconf3_cell) {
    triple[] edge_intersections;
    for (int i1 = 0; i1 < cell1.length; ++i1) {
      for (int j1 = i1 + 1; j1 < cell1.length; ++j1) {
	triple u1 = this.pointconf3_allpoints[cell1[i1]].point3_coord;
	triple v1 = this.pointconf3_allpoints[cell1[j1]].point3_coord;
	path3 p1 = arc(O, u1, v1, cross(u1, v1));
	for (int i2 = 0; i2 < cell2.length; ++i2) {
	  for (int j2 = i2 + 1; j2 < cell2.length; ++j2) {
	    triple u2 = this.pointconf3_allpoints[cell2[i2]].point3_coord;
	    triple v2 = this.pointconf3_allpoints[cell2[j2]].point3_coord;

	    if (((cell1[i1] != cell2[i2]) || (cell1[j1] != cell2[j2])) && ((cell1[i1] != cell2[j2]) || (cell1[j1] != cell2[i2]))) {
	      if ((cell1[i1] == cell2[i2]) || (cell1[i1] == cell2[j2])) {
		edge_intersections.push(u1);
	      }
	      else if ((cell1[j1] == cell2[i2]) || (cell1[j1] == cell2[j2])) {
		edge_intersections.push(v1);
	      }
	      else {
		path3 p2 = arc(O, u2, v2, cross(u2, v2));
		triple[] new_intersections = intersectionpoints(p1, p2);
		for (triple x : new_intersections) {
		  edge_intersections.push(x);
		}
	      }
	    }
	  }
	}
      }
    }
    return edge_intersections;
  }

  path3[] chambercone_path3array(real radius) {
    path3[] result;
    int[] cell = this.pointconf3_cell;
    for (int i = 0; i < cell.length; ++i) {
      for (int j = i + 1; j < cell.length; ++j) {
	triple xi = this.pointconf3_allpoints[cell[i]].point3_coord;
	triple xj = this.pointconf3_allpoints[cell[j]].point3_coord;
	if ((unit(xi) == unit(xj)) || (unit(xi) == -unit(xj))) {
	  continue;
	}
	path3 p3 = this.dimtwo_simplicialcone(cell[i], cell[j], radius);
	result.append(p3);
      }
    }
    return result;
  }
  
  surface chambercone_surface(real radius,
			      pen pointconf_pen = default_pointconf3_pen,
			      pen pointconf_fillpen = default_pointconf3_fillpen) {
    surface result;
    path3[] result_path3array = this.chambercone_path3array(radius);
    for (path3 p3 : result_path3array) {
      result.append(surface(p3, planar=true));
    }
    return result;
  }

  void draw_chambercone(picture pic = currentpicture,
			int i1,
			int i2,
			real radius,
			pen pointconf_pen = default_pointconf3_pen,
			pen pointconf_fillpen = default_pointconf3_fillpen) {
    path3 p3 = this.dimtwo_simplicialcone(i1, i2, radius);
    draw(pic, surface(p3), meshpen=nullpen, surfacepen=pointconf_fillpen);
    draw(pic, p3, p=pointconf_pen, light=currentlight);
  }

  void draw_chambercone(face[] facelist,
			int i1,
			int i2,
			real radius,
			pen pointconf_pen = default_pointconf3_pen,
			pen pointconf_fillpen = default_pointconf3_fillpen) {
    path3 p3 = this.dimtwo_simplicialcone(i1, i2, radius);
    filldraw(facelist.push(p3), project(p3), drawpen=pointconf_pen, fillpen=pointconf_fillpen);
  }
  
  void draw_chambercones(picture pic = currentpicture,
			 real radius,
			 pen pointconf_pen = default_pointconf3_pen,
			 pen pointconf_fillpen = default_pointconf3_fillpen) {
    draw(pic, this.chambercone_surface(radius), meshpen=nullpen, surfacepen=pointconf_fillpen);
    path3[] result_path3array = this.chambercone_path3array(radius);
    for (path3 p3 : result_path3array) {
      draw(pic, p3, p=pointconf_pen, light=currentlight);
    }
  }

  void draw_chambercones(face[] facelist,
			 real radius,
			 pen pointconf_pen = default_pointconf3_pen,
			 pen pointconf_fillpen = default_pointconf3_fillpen) {
    path3[] result_path3array = this.chambercone_path3array(radius);
    for (path3 p3 : result_path3array) {
      filldraw(facelist.push(p3), project(p3), drawpen=pointconf_pen, fillpen=lightcolor(p3, pointconf_fillpen));
    }
  }

  void draw_facets(picture pic = currentpicture,
		   pen pointconf_pen = default_pointconf3_pen,
		   pen pointconf_fillpen = default_pointconf3_fillpen) {
    surface boundary;
    path3[] boundary_path3array;
    write("Start boundary surface no. of patches:", boundary.s.length);
    boundary.append(this.boundary_surface());
    boundary_path3array.append(this.boundary_path3array());
    write("boundary surface no. of patches:", boundary.s.length);
    write("drawing boundary surface with pen ", pointconf_pen);
    draw(pic, boundary, meshpen=nullpen, surfacepen=pointconf_fillpen, light=currentlight);
    draw(pic, boundary_path3array, p=pointconf_pen, light=currentlight);
    write("... done.");
  }

  void draw_facets(face[] facelist,
		   pen pointconf_pen = default_pointconf3_pen,
		   pen pointconf_fillpen = default_pointconf3_fillpen) {
    path3[] boundary_path3array;
    write("Start facelist at no. ", facelist.length);
    boundary_path3array.append(this.boundary_path3array());
    write("drawing facets with pen ", pointconf_pen);
    for (path3 p3 : boundary_path3array) {
      filldraw(facelist.push(p3), project(p3), drawpen=pointconf_pen, fillpen=lightcolor(p3, pointconf_fillpen));
    }
    write("facelist at no. ", facelist.length);
    write("... done.");
  }

  void draw_points(picture pic = currentpicture,
		   triple point_normal=default_point_normal(),
		   int[] visiblepoints = this.pointconf3_cell,
		   real point_size = default_point3_size,
		   pen point_pen = default_point3_pen,
		   pen point_fillpen = default_point3_fillpen) {
    // draw points if requested:
    for (int i : visiblepoints) {
      labeled_point3 p = this.pointconf3_allpoints[i];
      p.draw_point(pic,
		   point_normal=point_normal, 
		   point_size=point_size, 
		   point_pen=point_pen, 
		   point_fillpen=point_fillpen);
    }
  }

  void draw_points(face[] facelist,
		   triple point_normal=default_point_normal(),
		   int[] visiblepoints = this.pointconf3_cell,
		   real point_size = default_point3_size,
		   pen point_pen = default_point3_pen,
		   pen point_fillpen = default_point3_fillpen) {
    // draw points if requested:
    for (int i : visiblepoints) {
      labeled_point3 p = this.pointconf3_allpoints[i];
      p.draw_point(facelist,
		   point_normal=point_normal, 
		   point_size=point_size, 
		   point_pen=point_pen, 
		   point_fillpen=point_fillpen);
    }
  }

  void draw_vectors(picture pic = currentpicture,
		    int[] visiblepoints = this.pointconf3_cell,
		    pen vector_pen = default_vector_pen) {
    // draw vectors if requested:
    for (int i : visiblepoints) {
      labeled_point3 p = this.pointconf3_allpoints[i];
      p.draw_vector(pic, vector_pen=vector_pen);
    }
    labeled_point3 origin = labeled_point3(O);
    origin.draw(pic, point_size=default_point3_size/2, point_pen=vector_pen, point_fillpen=vector_pen);
  }

  void draw_vectors(face[] facelist,
		    int[] visiblepoints = this.pointconf3_cell,
		    pen vector_pen = default_vector_pen) {
    // draw vectors if requested:
    for (int i : visiblepoints) {
      labeled_point3 p = this.pointconf3_allpoints[i];
      p.draw_vector(facelist, vector_pen=vector_pen);
    }
    labeled_point3 origin = labeled_point3(O);
    origin.draw(facelist, point_normal=Z, point_size=default_point3_size/2, point_pen=vector_pen, point_fillpen=vector_pen);
  }

  void draw_unitvectors(picture pic = currentpicture,
			real radius,
			int[] visiblepoints = this.pointconf3_cell,
			pen vector_pen = default_vector_pen) {
    // draw vectors if requested:
    for (int i : visiblepoints) {
      labeled_point3 p;
      p.point3_label = this.pointconf3_allpoints[i].point3_label;
      p.point3_coord = radius * unit(this.pointconf3_allpoints[i].point3_coord);
      p.draw_unitvector(pic, radius, vector_pen=vector_pen);
    }
    labeled_point3 origin = labeled_point3(O);
    origin.draw(pic, point_normal=Z, point_size=default_point3_size/2, point_pen=vector_pen, point_fillpen=vector_pen);
  }

  void draw_unitvectors(face[] facelist,
			real radius,
			int[] visiblepoints = this.pointconf3_cell,
			pen vector_pen = default_vector_pen) {
    // draw vectors if requested:
    for (int i : visiblepoints) {
      labeled_point3 p;
      p.point3_label = this.pointconf3_allpoints[i].point3_label;
      p.point3_coord = radius * unit(this.pointconf3_allpoints[i].point3_coord);
      p.draw_unitvector(facelist, radius, vector_pen=vector_pen);
    }
    labeled_point3 origin = labeled_point3(O);
    origin.draw(facelist, point_normal=Z, point_size=default_point3_size/2, point_pen=vector_pen, point_fillpen=vector_pen);
  }

  void draw_facetnormals(picture pic = currentpicture,
			 int[][] facets = this.pointconf3_facets,
			 pen vector_pen = default_vector_pen,
			 real normal_length = default_normal_length) {
    for (int[] facet : facets) {
      if (facet.length > 2) {
	triple normal_point = this.average_position(facet);
	triple normal_dir = this.facet_normal(facet);
	draw(pic, normal_point--normal_point + normal_length * normal_dir, p=vector_pen, arrow=Arrow3(DefaultHead2));
      }
    }
  }

  void draw_facetnormals(face[] facelist,
			 int[][] facets = this.pointconf3_facets,
			 pen vector_pen = default_vector_pen,
			 real normal_length = default_normal_length) {
    for (int[] facet : facets) {
      if (facet.length > 2) {
	triple normal_point = this.average_position(facet);
	triple normal_dir = this.facet_normal(facet);
	path3 p3 = normal_point--normal_point + normal_length * normal_dir;
	draw(facelist.push(p3), p3, p=vector_pen, arrow=Arrow3(DefaultHead2));
      }
    }
  }
  
  void draw_pointlabels(picture pic = currentpicture,
			triple global_up=Z,
			triple point_normal=default_point_normal(),
			transform label_transform=identity(),
			int[] visiblepoints = this.pointconf3_cell,
			real point_size = default_point3_size,
			pen point_pen = default_point3_pen,
			pen point_fillpen = default_point3_fillpen) {
    
    // draw labels of visible points:
    for (int i : visiblepoints) {
      labeled_point3 p = this.pointconf3_allpoints[i];
      p.draw_label(pic,
		   global_up=global_up,
		   point_normal=point_normal, 
		   label_transform=label_transform,
		   point_size=point_size, 
		   point_pen=point_pen, 
		   point_fillpen=point_fillpen);
    }
  }

  void draw_pointlabels(face[] facelist,
			triple global_up=Z,
			triple point_normal=default_point_normal(),
			transform label_transform=identity(),
			int[] visiblepoints = this.pointconf3_cell,
			real point_size = default_point3_size,
			pen point_pen = default_point3_pen,
			pen point_fillpen = default_point3_fillpen) {
    
    // draw labels of visible points:
    for (int i : visiblepoints) {
      labeled_point3 p = this.pointconf3_allpoints[i];
      p.draw_label(facelist,
		   global_up=global_up,
		   point_normal=point_normal, 
		   label_transform=label_transform,
		   point_size=point_size, 
		   point_pen=point_pen, 
		   point_fillpen=point_fillpen);
    }
  }

  void draw_pointconflabel(picture pic = currentpicture,
			   triple global_up=Z,
			   triple label3_position = default_label3_position(),
			   triple label3_normal = default_point_normal(),
			   transform label_transform = identity(),
			   align label_align = this.pointconf3_label.align,
			   pen pointconf_pen=default_pointconf3_pen) {
    label(pic=pic, 
	  L=this.pointconf3_label,
	  global_up=global_up,
	  label3_position=label3_position,
	  label3_normal=label3_normal,
	  label_transform=label_transform,
	  label_align=label_align, 
	  p=pointconf_pen,
	  P=get_currentprojection());
  }

  void draw_pointconflabel(face[] facelist,
			   triple global_up=Z,
			   triple label3_position = default_label3_position(),
			   triple label3_normal = default_point_normal(),
			   transform label_transform = identity(),
			   align label_align = this.pointconf3_label.align,
			   pen pointconf_pen=default_pointconf3_pen) {
    label(facelist, 
	  L=this.pointconf3_label,
	  global_up=global_up,
	  label3_position=label3_position,
	  label3_normal=label3_normal,
	  label_transform=label_transform,
	  label_align=label_align, 
	  p=pointconf_pen,
	  P=get_currentprojection());
  }

  void draw(picture pic = currentpicture,
	    triple global_up=Z,
	    triple label3_normal = default_label3_normal(),
	    triple label3_position = default_label3_position(),
	    transform pointconflabel_transform = identity(),
	    align label_align = this.pointconf3_label.align,
	    triple point_normal = default_point_normal(),
	    transform pointlabel_transform = identity(),
	    int[] visiblepoints = this.pointconf3_cell,
	    pen pointconf_pen = default_pointconf3_pen,
	    pen pointconf_fillpen = default_pointconf3_fillpen,
	    real point_size = default_point3_size,
	    pen point_pen = default_point3_pen,
	    pen point_fillpen = default_point3_fillpen) {
    write("drawing facets ...");
    draw_facets(pic, pointconf_pen, pointconf_fillpen);
    write("... done.");
    write("drawing points ...");
    draw_points(pic, point_normal, visiblepoints, point_size, point_pen, point_fillpen);
    write("... done.");
    write("drawing point labels ...");
    draw_pointlabels(pic, global_up, point_normal, pointlabel_transform, visiblepoints, point_size, point_pen, point_fillpen);
    write("... done.");
    write("drawing pointconf label ...");
    draw_pointconflabel(pic,
			global_up=global_up,
			label3_normal=label3_normal, 
			label3_position = label3_position, 
			label_transform = pointconflabel_transform, 
			label_align = label_align,
			pointconf_pen);
    write("... done.");
  }

  void draw(face[] facelist,
	    triple global_up=Z,
	    triple label3_normal = default_label3_normal(),
	    triple label3_position = default_label3_position(),
	    transform pointconflabel_transform = identity(),
	    align label_align = this.pointconf3_label.align,
	    triple point_normal = default_point_normal(),
	    transform pointlabel_transform = identity(),
	    int[] visiblepoints = this.pointconf3_cell,
	    pen pointconf_pen = default_pointconf3_pen,
	    pen pointconf_fillpen = default_pointconf3_fillpen,
	    real point_size = default_point3_size,
	    pen point_pen = default_point3_pen,
	    pen point_fillpen = default_point3_fillpen) {
    write("drawing facets ...");
    draw_facets(facelist, pointconf_pen, pointconf_fillpen);
    write("... done.");
    write("drawing points ...");
    draw_points(facelist, point_normal, visiblepoints, point_size, point_pen, point_fillpen);
    write("... done.");
    write("drawing point labels ...");
    draw_pointlabels(facelist, global_up, point_normal, pointlabel_transform, visiblepoints, point_size, point_pen, point_fillpen);
    write("... done.");
    write("drawing pointconf label ...");
    draw_pointconflabel(facelist,
			global_up=global_up,
			label3_normal=label3_normal, 
			label3_position = label3_position, 
			label_transform = pointconflabel_transform, 
			label_align = label_align,
			pointconf_pen);
    write("... done.");
  }
};

void draw(picture pic=currentpicture,
	  pointconf3 pc3,
	  triple global_up=Z,
	  triple label3_normal = default_label3_normal(),
	  triple label3_position = default_label3_position(), 
	  transform pointconflabel_transform = identity(),
	  align label_align = pc3.pointconf3_label.align,
	  triple point_normal=default_point_normal(),
	  transform pointlabel_transform = identity(),
	  int[] visiblepoints = pc3.pointconf3_cell, 
	  pen pointconf_pen=default_pointconf3_pen,
	  pen pointconf_fillpen=default_pointconf3_fillpen,
	  real point_size=default_point3_size,
	  pen point_pen=default_point3_pen,
	  pen point_fillpen=default_point3_fillpen) {
  pc3.draw(pic=pic, 
	   global_up=global_up,
	   label3_normal=label3_normal, 
	   label3_position=label3_position, 
	   pointconflabel_transform=pointconflabel_transform,
	   label_align = label_align,
	   point_normal=point_normal, 
	   pointlabel_transform=pointlabel_transform,
	   visiblepoints=visiblepoints, 
	   pointconf_pen=pointconf_pen, 
	   pointconf_fillpen=pointconf_fillpen, 
	   point_size=point_size, 
	   point_pen=point_pen, 
	   point_fillpen=point_fillpen);
}
  
void draw(face[] facelist,
	  pointconf3 pc3,
	  triple global_up=Z,
	  triple label3_normal = default_label3_normal(),
	  triple label3_position = default_label3_position(), 
	  transform pointconflabel_transform = identity(),
	  align label_align = pc3.pointconf3_label.align,
	  triple point_normal=default_point_normal(),
	  transform pointlabel_transform = identity(),
	  int[] visiblepoints = pc3.pointconf3_cell, 
	  pen pointconf_pen=default_pointconf3_pen,
	  pen pointconf_fillpen=default_pointconf3_fillpen,
	  real point_size=default_point3_size,
	  pen point_pen=default_point3_pen,
	  pen point_fillpen=default_point3_fillpen) {
  pc3.draw(facelist, 
	   global_up=global_up,
	   label3_normal=label3_normal, 
	   label3_position=label3_position, 
	   pointconflabel_transform=pointconflabel_transform,
	   label_align = label_align,
	   point_normal=point_normal, 
	   pointlabel_transform=pointlabel_transform,
	   visiblepoints=visiblepoints, 
	   pointconf_pen=pointconf_pen, 
	   pointconf_fillpen=pointconf_fillpen, 
	   point_size=point_size, 
	   point_pen=point_pen, 
	   point_fillpen=point_fillpen);
}

pointconf project(pointconf3 pc3, projection P=orthographic(Z), int[] extremepoints={}) {
  pointconf result = new pointconf;
  result.pointconf_label = pc3.pointconf3_label;
  for (labeled_point3 p3 : pc3.pointconf3_allpoints) {
    labeled_point p = project(p3, P);
    result.pointconf_allpoints.push(p);
  }
  for (int i : pc3.pointconf3_cell) {
    result.pointconf_cell.push(i);
  }
  for (int i : extremepoints) {
    result.pointconf_extremepoints.push(i);
  }
  return result;
}

pointconf operator * (transform t, pointconf A) {
  for (labeled_point p : A.pointconf_allpoints) {
    p.point_coord = t * p.point_coord;
  }
  return A;
}

pointconf3 operator * (transform3 t, pointconf3 A) {
  for (labeled_point3 p : A.pointconf3_allpoints) {
    p.point3_coord = t * p.point3_coord;
  }
  A.pointconf3_label.position = project(t * invert(A.pointconf3_label.position, Z, Z, get_currentprojection()), P=get_currentprojection());
  return A;
}

// construct a 3D configuration by homogenization to a plane through O perpendicular to normal:
pointconf3 homogenize(pointconf A, triple normal=Z, triple point=Z * cm) {
  pointconf3 result;
  result.pointconf3_label = A.pointconf_label;
  for (labeled_point p : A.pointconf_allpoints) {
    labeled_point3 p3 = homogenize(p, normal, point);
    result.pointconf3_allpoints.push(p3);
  }
  result.pointconf3_cell = copy(A.pointconf_cell);
  result.pointconf3_facets.push(A.pointconf_extremepoints);
  return result;
}


// Triangulations:
pointconf3 simplex(pointconf3 A, int[] simplicial_cell, bool copy) {
  pointconf3 result;
  int[][] facets;
  for (int i = 0; i < simplicial_cell.length; ++i) {
    for (int j = i+1; j < simplicial_cell.length; ++j) {
      for (int k = j+1; k < simplicial_cell.length; ++k) {
	facets.push(new int[] {simplicial_cell[i],simplicial_cell[j],simplicial_cell[k]});      
      }
    }
  }
  return pointconf3(A, cell=simplicial_cell, facets=facets, copy);
}

struct triangulation3 {
  pointconf3 triangulation3_pointconf;
  int[][] triangulation3_cells;

  void operator init(triangulation3 t) {
    this.triangulation3_pointconf = t.triangulation3_pointconf;
    this.triangulation3_cells = t.triangulation3_cells;
  }
  
  void operator init(pointconf3 A, int[][] cells) {
    this.triangulation3_pointconf = A;
    this.triangulation3_cells = cells;
  }
  
  pointconf3[] pointconfs(triple explode_center,
			  real explode_factor) {
    pointconf3[] result;
    for (int[] simplicial_cell : this.triangulation3_cells) {
      pointconf3 S = simplex(this.triangulation3_pointconf, simplicial_cell, copy=true);
      triple shift_vector = S.average_position() - explode_center;
      result.push(shift((explode_factor - 1) * shift_vector) * S);
    }
    return result;
  }
};

// special point configurations:

pointconf cyclic_moment(int no_of_points) {
  pointconf result = new pointconf;
  result.pointconf_label = Label(format("$\mathbf{C}(%d, 2)$", no_of_points), Relative(0.0), align=2*S);
  for (int i = 0; i < no_of_points; ++i) {
    labeled_point p;
    p.point_label = Label(format("$%d$", i+1));
    p.point_coord = (i * cm, i^2 * cm);
    result.pointconf_allpoints.push(p);
    result.pointconf_cell.push(i);
    result.pointconf_extremepoints.push(i);
  }
  return result;
}

pointconf cyclic_caratheodory(int no_of_points) {
  pointconf result = new pointconf;
  result.pointconf_label = Label(format("$\mathbf{C}(%d, 2)$", no_of_points), Relative(0.0), align=2*S);
  for (int i = 0; i < no_of_points; ++i) {
    labeled_point p;
    p.point_label = Label(format("$%d$", i+1));
    p.point_coord = -(sin(2 * i/no_of_points * pi) * cm, cos(2 * i/no_of_points * pi) * cm);
    result.pointconf_allpoints.push(p);
    result.pointconf_cell.push(i);
    result.pointconf_extremepoints.push(i);
  }
  return result;
}

pointconf3 homogeneous_mother_of_all_examples() {
  triple[] moae_homogeneous_coords = {(4cm,0cm,0cm),(0cm,4cm,0cm),(0cm,0cm,4cm),(2cm,1cm,1cm),(1cm,2cm,1cm),(1cm,1cm,2cm)};
  int[][] moae_facets = {{0,1,2}};
  pointconf3 result = pointconf3(Label("$\mathbf{M}$"),
				 moae_homogeneous_coords, 
				 facets=moae_facets);
  return result;
}

pointconf mother_of_all_examples(real twist=0,
				 Label L=Label("$\mathbf{M}$", position=project(O, orthographic(1,1,1)), align=Center)) {
  if (abs(twist) > 59) {
    abort("twist too large - must be in [-59, +59]");
  }
  triple[] moae_homogeneous_coords_outer = {(4cm,0cm,0cm),(0cm,4cm,0cm),(0cm,0cm,4cm)};
  triple[] moae_homogeneous_coords_inner = {(2cm,1cm,1cm),(1cm,2cm,1cm),(1cm,1cm,2cm)};
  pair[] moae_coords = rotate(twist) * project(moae_homogeneous_coords_outer, orthographic(1,1,1));
  moae_coords.append(project(moae_homogeneous_coords_inner, orthographic(1,1,1)));
  int[] moae_extremepoints = {0,1,2};
  pointconf result = pointconf(L, 
			       moae_coords, 
			       extremepoints=moae_extremepoints);
  return result;
}

pointconf3 schoenhardt(int[][] facets=new int[][] {{1,0,3},{2,1,4},{0,2,5},{1,3,4},{2,4,5},{0,5,3},{0,1,2},{5,4,3}},
		       real twist = 10,
		       real lift = 1cm,
		       real scale_top = 1,
		       real scale_bottom = 1,
		       Label L=Label("$\textbf{Schönhardt}$", position=project(Z*lift/2, orthographic(Z)), align=Center)) {
  if (abs(twist) > 59) {
    abort("twist too large - must be in [-59, +59]");
  }
  triple[] homogeneous_base = {(4cm,0cm,0cm), (0cm,4cm,0cm), (0cm,0cm,4cm)};
  pair[] base = rotate(-twist/2) * project(homogeneous_base, orthographic(1,1,1));
  pair[] top = copy(base);
  base = scale(scale_bottom) * base;
  top = scale(scale_top) * rotate(twist) * top;
  triple[] schoenhardt_coords = new triple[];
  for (pair coord : top) {
    triple next_coord = invert(coord, Z, Z * lift/2, P=orthographic(Z));
    schoenhardt_coords.push(next_coord);
  }
  for (pair coord : base) {
    triple next_coord = invert(coord, Z, -Z * lift/2, P=orthographic(Z));
    schoenhardt_coords.push(next_coord);
  }
  int[][] schoenhardt_facets = facets;
  pointconf3 result = pointconf3(L,
                                 schoenhardt_coords, 
                                 facets=schoenhardt_facets);
  return result;
}
// eof
