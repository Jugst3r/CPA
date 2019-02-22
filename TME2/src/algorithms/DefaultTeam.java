package algorithms;

import java.awt.Point;
import java.util.ArrayList;
import java.util.Comparator;
import java.util.HashMap;

import javax.management.RuntimeErrorException;

import algorithms.DefaultTeam.Arete;


public class DefaultTeam {
	
	public static class PointIdent {
		static int NEX_ID = 0;
		private int clusterId = NEX_ID++;
		private Point pt;

		public PointIdent(Point p) {
			pt = p;
		}

		public int getId() {
			return clusterId;
		}

		public Point getPoint() {
			return pt;
		}

		public String toString() {
			return pt.toString();
		}
	}
	
	public class Arete  {

		private PointIdent p;
		private PointIdent p2;
		private double poids;

		public Arete(PointIdent p, PointIdent p2, double poids) {
			this.p = p;
			this.p2 = p2;
			this.poids = poids;
		}

		public PointIdent getP() {
			return p;
		}


		public PointIdent getQ() {
			return p2;
		}

		public double poids() {
			return this.poids;
		}
	}

	public ArrayList<Object> floydWarshall(ArrayList<Point> points, int edgeThreshold) {

		int n = points.size();
		double[][] M = new double[n][n];

		int[][] R = new int[n][n];

		for (int i = 0; i < n; i++) {
			for (int j = 0; j < n; j++) {
				double dist = points.get(i).distance(points.get(j));
				M[i][j] = dist < edgeThreshold ? dist : Double.POSITIVE_INFINITY;
				R[i][j] = j;
				// System.out.println("R[i][j] vaut " + R[i][j]);
			}
		}

		for (int p = 0; p < n; p++) {
			for (int i = 0; i < n; i++) {
				for (int j = 0; j < n; j++) {
					if (M[i][j] > M[i][p] + M[p][j]) {
						M[i][j] = M[i][p] + M[p][j];
						R[i][j] = R[i][p];
					}
				}
			}
		}
		ArrayList<Object> result = new ArrayList<>();
		result.add(M);
		result.add(R);
		return result;
	}

	public int[][] calculShortestPaths(ArrayList<Point> points, int edgeThreshold) {

		int n = points.size();
		double[][] M = new double[n][n];

		int[][] R = new int[n][n];

		for (int i = 0; i < n; i++) {
			for (int j = 0; j < n; j++) {
				double dist = points.get(i).distance(points.get(j));
				M[i][j] = dist < edgeThreshold ? dist : Double.POSITIVE_INFINITY;
				R[i][j] = j;
				// System.out.println("R[i][j] vaut " + R[i][j]);
			}
		}

		for (int p = 0; p < n; p++) {
			for (int i = 0; i < n; i++) {
				for (int j = 0; j < n; j++) {
					if (M[i][j] > M[i][p] + M[p][j]) {
						M[i][j] = M[i][p] + M[p][j];
						R[i][j] = R[i][p];
					}
				}
			}
		}
		return R;
	}
	
	public ArrayList<Arete> kruskal(ArrayList<Point> points2, ArrayList<Point> points3,double [][] M) {

		ArrayList<Arete> aretes = new ArrayList<Arete>();

		ArrayList<PointIdent> points = new ArrayList<>();
		for (Point pt : points2) {
			points.add(new PointIdent(pt));
		}

		int cpt = 0;
		for (int i = 0; i < points.size(); i++) {
			for (int j = i + 1; j < points.size(); j++) {
				cpt++;
				System.out.println("la distance entre " + points.get(i) + " et " + points.get(j) + " est " + M[i][j]);
				int indX = points3.indexOf(points2.get(i));
				int indY = points3.indexOf(points2.get(j));
				Arete a = new Arete(points.get(i), points.get(j), M[indX][indY]);
				aretes.add(a);
			}
		}
		System.out.println("cpt vaut " + cpt);
		aretes.sort(Comparator.comparingDouble(Arete::poids));

		ArrayList<Arete> sol = new ArrayList<Arete>();
		int i = 0;
		while (sol.size() < points.size() - 1) {
			Arete ar = aretes.get(i++);
			int id1 = ar.getP().clusterId;
			int id2 = ar.getQ().clusterId;

			if (id1 != id2) {
				sol.add(ar);

				for (PointIdent pt : points) {
					if (pt.clusterId == id2) {
						pt.clusterId = id1;
					}
				}
			}

		}

		return sol;

	}
	
	HashMap<Arete, Boolean> test = new HashMap<>();
	int cpt = 0;
	
	
	
	
	
	public Tree2D aretesToTree(Point p, ArrayList<Arete> aretes) {
		ArrayList<Arete> newArete = (ArrayList<Arete>) aretes.clone();
		ArrayList<Point> succ = new ArrayList<>();

		for (Arete a : aretes) {
			
			/*
			if(test.containsKey(a)) {
				System.out.println("oheee");;
			}
			*/
			test.put(a, true);
			//System.out.println(a);
			if (a.getP().getPoint().equals(p)) {
				succ.add(a.getQ().getPoint());
				newArete.remove(a);
			} else {
				if (a.getQ().getPoint().equals(p)) {
					succ.add(a.getP().getPoint());
					newArete.remove(a);
				}
			}
		}

		ArrayList<Tree2D> subtrees = new ArrayList<>();

		for (Point q : succ) {
			subtrees.add(aretesToTree(q, newArete));
		}
		return new Tree2D(p, subtrees);
	}

	public Tree2D areteToTree(ArrayList<Arete> aretes) {
		return aretesToTree(aretes.get(0).getP().pt, aretes);
	}

	
	public ArrayList<Arete> replace (Arete a, ArrayList<Point> points, int[][] paths){
		PointIdent p = a.getP();
		PointIdent q = a.getQ();
		int nbP = -1;
		int nbQ = -1;
		
		for(int i = 0; i < points.size(); i++) {
			if(points.get(i).equals(p.pt)) {
				nbP = i;
			}
			if(points.get(i).equals(q.pt)) {
				nbQ = i;
			}
			if(nbP != -1 && nbQ != -1)
				break;
		}
		
		ArrayList<Arete> res = new ArrayList<>();
		
		while(nbP != nbQ) {
			Point p1 = points.get(nbP);
			Point p2 = points.get(paths[nbP][nbQ]);
			double dist = p1.distance(p2);
			res.add(new Arete(new PointIdent(p1), new PointIdent(p2), dist));
			nbP = paths[nbP][nbQ];

			
		}
		
		return res;
		
	}
	public Tree2D calculSteiner(ArrayList<Point> points, int edgeThreshold, ArrayList<Point> hitPoints) {
		// REMOVE >>>>>

		Tree2D leafX = new Tree2D(new Point(700, 400), new ArrayList<Tree2D>());
		Tree2D leafY = new Tree2D(new Point(700, 500), new ArrayList<Tree2D>());
		Tree2D leafZ = new Tree2D(new Point(800, 450), new ArrayList<Tree2D>());
		ArrayList<Tree2D> subTrees = new ArrayList<Tree2D>();
		subTrees.add(leafX);
		subTrees.add(leafY);
		subTrees.add(leafZ);
		Tree2D steinerTree = new Tree2D(new Point(750, 450), subTrees);
		// <<<<< REMOVE

		ArrayList<Object> result = floydWarshall(points, edgeThreshold);
		
		

		double dist[][] = (double[][]) result.get(0);
		int paths[][] = (int[][]) result.get(1);
		ArrayList<Arete> krusk = kruskal(hitPoints, points, dist);
		ArrayList<Arete> kruskafter = new ArrayList<Arete>();
		for(Arete a: krusk) {
			kruskafter.addAll(replace(a, points, paths));
		}
		System.out.println(kruskafter.size());
		System.out.println("salut22");
		Tree2D T0 = areteToTree(kruskafter);
		System.out.println("salut22fdsfds");
		
		System.out.println(T0);
		return T0;
	}
	
	/*
	public Tree2D calculSteinerM(ArrayList<Point> points, int edgeThreshold, ArrayList<Point> hitPoints, Point mere) {
		
		for (int i = 0;i<10;i++) {
			calculSteiner(points,edgeThreshold,hitPoints);
		}
		
	}*/
	
}
