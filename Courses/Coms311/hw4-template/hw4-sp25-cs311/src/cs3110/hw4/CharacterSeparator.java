package cs3110.hw4;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.stream.Collectors;
import java.util.stream.IntStream;
import java.util.Map;

public class CharacterSeparator {
    /**
     * This method uses the WeightedAdjacencyList class to identify the space between characters in an image of text.
     * For efficiency, it should only construct a single graph object and should only make a constant
     * number of calls to Dijkstra's algorithm.
     * @param path The location of the image on disk.
     * @return Two lists of Integer. The first list indicates whitespace rows. The second list indicates whitespace columns. Returns null if some error occurred loading the image.
     */
    public static Pair<List<Integer>, List<Integer>> findSeparationWeighted(String path) {
        try {

            // Need to load the image
            //Load the image from the path and convert into array of pixel vals
            BitmapProcessor processor = new BitmapProcessor(path);
            
            //Now convert to 2d array
            int[][] pixelMatrix = processor.getRGBMatrix();
            

            // Get dimensions of the image 
            int height = pixelMatrix.length;//technically number of rows
            //height and length

            int width = pixelMatrix[0].length;//columns
            

            // Now need to create the vertices for the graph
            // each vertex will be considered a pair (row and column)
            // The coords for the pixel
            List<Pair<Integer, Integer>> vertices = new ArrayList<>();

            //Go through each pixel 
            for (int row = 0; row < height; row++) {
                
                for (int column = 0; column < width; column++) {
                    //add each coordinate of each pixel as a vertex
                    vertices.add(new Pair<>(row, column));// returning the coords 
                }
            }
            
            //////////////////////////////////////////////////////////////////////
            // Now on to creating the graph (This will be where each vertex is (integer,integer))
            // --> (row, column)
        
            WeightedAdjacencyList<Pair<Integer, Integer>> graph = new WeightedAdjacencyList<>(vertices);
            
            // based on the pixel colors we need to add edges to the graph.
            // each pixel is assigned a weight. and adjacement pizels are connected with these weights
            //determined by color

            //Now need to add edges between the adjacent pixels where each edge will have weight based on the color as I said above
            for (int row = 0; row < height; row++) {
                for (int column = 0; column < width; column++) {
                    //Go through and get the current pixel as a vertex from how we stored it above
                    Pair<Integer, Integer> current = new Pair<>(row, column);
                    
                    //The next statements will be connecting to different neighbors using if statements to determine which

                    //This if statement will connect to the right neighbor if it is there

                    if (column + 1 < width) {
                        //column + 1
                        //create a vertex for the right neighbor
                        Pair<Integer, Integer> right = new Pair<>(row, column + 1);
                        //calc weight based on the neihbors
                        int weight = calculateWeight(pixelMatrix[row][column], pixelMatrix[row][column + 1]);
                        //add edge from the current to the right
                        graph.addEdge(current, right, weight);
                        //add from right to the current 
                        graph.addEdge(right, current, weight); 
                        //This would make it both ways and undirected
                    }
                    
                    //These will all be the same structure just with the different neighbors
                    // Connect to bottom neighbor if it exists
                    if (row + 1 < height) {
                        //create vertex for bottom
                        Pair<Integer, Integer> bottom = new Pair<>(row + 1, column);
                        //calc the weight 
                        int weight = calculateWeight(pixelMatrix[row][column], pixelMatrix[row + 1][column]);

                        //Curent to bottom, bottom to current (undirected)
                        graph.addEdge(current, bottom, weight);
                        graph.addEdge(bottom, current, weight); 
                    }
                    
                    // Connect to bottom right 
                    if (row + 1 < height && column + 1 < width) {
                        //create vert
                        Pair<Integer, Integer> diagonal = new Pair<>(row + 1, column + 1);
                        //weigth
                        int weight = calculateWeight(pixelMatrix[row][column], pixelMatrix[row + 1][column + 1]);
                        graph.addEdge(current, diagonal, weight);
                        graph.addEdge(diagonal, current, weight); 
                        //connect back and forth
                    }
                    
                    // Connect to bottom left --> same thing but you know left and not right
                    if (row + 1 < height && column - 1 >= 0) {

                        Pair<Integer, Integer> diagonal = new Pair<>(row + 1, column - 1);
                        int weight = calculateWeight(pixelMatrix[row][column], pixelMatrix[row + 1][column - 1]);
                        graph.addEdge(current, diagonal, weight);
                        graph.addEdge(diagonal, current, weight);
                    }
                }
            }
            
            // Find horizontal separation --> this would be rows
            List<Integer> horizontalSeparations = findHorizontalSeparations(graph, height, width);
            
            // Find vertical separation --> columns
            List<Integer> verticalSeparations = findVerticalSeparations(graph, height, width);
            
            //return them as a pair
            return new Pair<>(horizontalSeparations, verticalSeparations);
            
        } catch (IOException e) {
            //if error loading produce an error, this will help me determine issues
            e.printStackTrace();
            return null;
        }
    }
    
    /**
     * Calculates the weight of an edge between two pixels based on their colors.
     * White pixels (0xFFFFFFFF) get low weights, darker pixels get higher weights.
     * 
     * @param pixel1 The first pixel
     * @param pixel2 The second pixel
     * @return The weight of the edge
     */
    private static int calculateWeight(int pixel1, int pixel2) {
        // Extract RGB values

        //extract the red component from first pixel and shift 16
        int r1 = (pixel1 >> 16) & 0xFF;
        //extract green component
        int g1 = (pixel1 >> 8) & 0xFF;
        //extract blue
        int b1 = pixel1 & 0xFF;
        
        //extract red from second pixel
        int r2 = (pixel2 >> 16) & 0xFF;
        //green
        int g2 = (pixel2 >> 8) & 0xFF;
        //blue
        int b2 = pixel2 & 0xFF;
        
        // Calculate brightness avg of the rgb vals i got from before

        int brightness1 = (r1 + g1 + b1) / 3;
        int brightness2 = (r2 + g2 + b2) / 3;
        
        // Average between both the brightness
        int avgBrightness = (brightness1 + brightness2) / 2;
        
        // Invert the brightness to get the weight 
        //(white = low weight, black = high weight) for note
        // makes the white pixels have a brightness of 255 have low weights
        //scale it 
       
        return 1000 - (avgBrightness * 1000 / 255);
    }
    
    /**
     * Finds horizontal separation lines 
     * 
     * @param graph The graph representation of the image
     * @param height The height of the image
     * @param width The width of the image
     * @return A list of row indices that represent separations between text lines
     */
    private static List<Integer> findHorizontalSeparations(
            WeightedAdjacencyList<Pair<Integer, Integer>> graph, int height,int width) {
        //create liste to store rows that are separations
        List<Integer> separations = new ArrayList<>();
        
        // For each row, we'll run Dijkstra's algorithm from the leftmost pixel to the rightmost pixel
        // If the shortest path has a low weight, it indicates a whitespace row

        for (int row = 0; row < height; row++) {
            //define left most as the source pixel
            Pair<Integer, Integer> source = new Pair<>(row, 0);
            //right most is the target
            Pair<Integer, Integer> target = new Pair<>(row, width - 1);
            
            // Run Dijkstra's algorithm to find the shortest path from the source
            Map<Pair<Integer, Integer>, Long> shortestPaths = graph.getShortestPaths(source);
            
            // Check if there's a path to the target and if its weight is low enough
            //lower weight means whiter path 
            if (shortestPaths.containsKey(target) && shortestPaths.get(target) < 1000 * width / 4) {
                // This row is likely a whitespace row
                //if the path is considered light the row would then be considered whitespace
                separations.add(row);
            }
        }
        
        return separations;
    }
    
    /**
     * Finds vertical separation lines 
     * 
     * @param graph The graph representation of the image
     * @param height The height of the image
     * @param width The width of the image
     * @return A list of column indices that represent separations between characters or words
     */
    private static List<Integer> findVerticalSeparations(
            WeightedAdjacencyList<Pair<Integer, Integer>> graph,int height,int width) {
        
        //much like before create a list to store the columns (not rows) that are separations
        List<Integer> separations = new ArrayList<>();
        
        // For each column, we'll run Dijkstra's algorithm from the topmost pixel to the bottommost pixel
        
        for (int column = 0; column < width; column++) {
            //top as source this time
            Pair<Integer, Integer> source = new Pair<>(0, column);
            //bottom as the target
            Pair<Integer, Integer> target = new Pair<>(height - 1, column);
            
            // Run Dijkstra's algorithm
            Map<Pair<Integer, Integer>, Long> shortestPaths = graph.getShortestPaths(source);
            
            // Check if there's a path to the target and if its weight is low enough
            //same determinant as a above for whitespace.
            if (shortestPaths.containsKey(target) && shortestPaths.get(target) < 1000 * height / 4) {
                // This column is likely a whitespace column
                separations.add(column);
            }
        }
        
        //return the list of separations.
        return separations;
    }
}
