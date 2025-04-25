package cs3110.hw4;

import java.io.File;
import java.util.*;

public class HW4Test {
    public static void main(String[] args) {
        // Test the graph implementation
        testGraph();
        
        // Print current working directory
        String currentDir = System.getProperty("user.dir");
        System.out.println("Current working directory: " + currentDir);
        
        // Construct the absolute path to the resources directory
        // From your directory structure: /home/becks/308/michael_becker_studies/Courses/Coms311/hw4-template/hw4-sp25-cs311/src/cs3110/hw4/resources/
        String resourcePath = currentDir + "/michael_becker_studies/Courses/Coms311/hw4-template/hw4-sp25-cs311/src/cs3110/hw4/resources/";
        
        // Check if directory exists
        File resourceDir = new File(resourcePath);
        if (!resourceDir.exists() || !resourceDir.isDirectory()) {
            System.out.println("Resource directory not found at: " + resourcePath);
            
            // Try alternative paths
            String[] alternativePaths = {
                currentDir + "/Courses/Coms311/hw4-template/hw4-sp25-cs311/src/cs3110/hw4/resources/",
                currentDir + "/michael_becker_studies/Courses/Coms311/hw4-template/resources/",
                "/home/becks/308/michael_becker_studies/Courses/Coms311/hw4-template/hw4-sp25-cs311/src/cs3110/hw4/resources/"
            };
            
            for (String path : alternativePaths) {
                File dir = new File(path);
                if (dir.exists() && dir.isDirectory()) {
                    resourcePath = path;
                    System.out.println("Found resources at: " + resourcePath);
                    break;
                }
            }
        } else {
            System.out.println("Found resources at: " + resourcePath);
        }
        
        // Test with the images using absolute paths
        System.out.println("\nTesting with the provided images:");
        testCharacterSeparator(resourcePath + "every_5th_column_white.bmp");
        testCharacterSeparator(resourcePath + "every_5th_row_white.bmp");
        testCharacterSeparator(resourcePath + "gradient_columns.bmp");
        testCharacterSeparator(resourcePath + "all_black.bmp");
        testCharacterSeparator(resourcePath + "all_white.bmp");
    }
    
    private static void testGraph() {
        // Your existing test graph method
        // ...
    }
    
    private static void testCharacterSeparator(String imagePath) {
        System.out.println("\nTesting with image: " + imagePath);
        
        try {
            // Run character separator
            Pair<List<Integer>, List<Integer>> result = 
                CharacterSeparator.findSeparationWeighted(imagePath);
            
            if (result == null) {
                System.out.println("Error processing image: " + imagePath);
                return;
            }
            
            // Print results
            List<Integer> rows = result.getFirst();
            List<Integer> columns = result.getSecond();
            
            System.out.println("Found " + rows.size() + " horizontal separations (rows)");
            if (rows.size() > 0) {
                System.out.println("First 10 rows: " + 
                    rows.subList(0, Math.min(10, rows.size())));
            }
            
            System.out.println("Found " + columns.size() + " vertical separations (columns)");
            if (columns.size() > 0) {
                System.out.println("First 10 columns: " + 
                    columns.subList(0, Math.min(10, columns.size())));
            }
            
        } catch (Exception e) {
            System.out.println("Exception: " + e.getMessage());
            e.printStackTrace();
        }
        
        System.out.println("---------------------------------------------------");
    }
}