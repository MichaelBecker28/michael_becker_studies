package cs3110.hw4;

import javax.imageio.ImageIO;
import javax.swing.*;
import java.awt.*;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;

/**
 * This class is not very robust and is relatively limited. It is used to read in a bitmap image and get the
 * corresponding grid of pixels.
 */
public final class BitmapProcessor {
    String originalPath;
    BufferedImage bi;

    /**
     * Constructs and instance of the class.
     * @param path The filename of an image to be loaded.
     * @throws IOException
     */
    public BitmapProcessor(String path) throws IOException {
        originalPath = path;
        bi = ImageIO.read(new File(path));
    }

    /**
     * Gets the grid of ARGB pixels (with transparency off).
     * @return A 2D array of pixels.
     */
    public int[][] getRGBMatrix() {
        final var h = bi.getTileHeight();
        final var w = bi.getTileWidth();
        int[][] rgbMat = new int[h][w];
        for (var i = 0; i < h; i++)
            for (var j = 0; j < w; j++)
                rgbMat[i][j] =  bi.getRGB(j, i) | 0xFF000000;
        return rgbMat;
    }

    /**
     * Updates the object with the given matrix.
     * @param mat A pixel matrix of the same dimensions this object's image.
     * @throws Exception If there is a dimension mismatch.
     */
    public void updateRGBMatrix(int[][] mat) throws Exception {
        final var h = bi.getTileHeight();
        final var w = bi.getTileWidth();

        if (mat.length != h || mat[0].length != w)
            throw new Exception("Dimension mismatch: Unable to update image matrix.");

        for (var i = 0; i < h; i++)
            for (var j = 0; j < w; j++)
                bi.setRGB(j, i, mat[i][j] | 0xFF000000);
    }

    /**
     * Write the current image to a poorly named file. Likely to overwrite preexisting images.
     * Possibly useful if you want to visualize any changes you've made to an image.
     * @throws IOException If there was an issue writing the image to a file.
     */
    public void writeToFile() throws IOException {
        ImageIO.write(bi, "bmp", new File(originalPath + ".new.bmp"));
    }

    /**
     * This (static) method can be used to show two images side-by-side.
     * @param before The first image to display.
     * @param after The second image to display.
     */
    public static void displayImages(BitmapProcessor before, BitmapProcessor after) {
        ImageIcon iconBefore = new ImageIcon(before.bi);
        ImageIcon iconAfter = new ImageIcon(after.bi);
        JLabel labelBefore = new JLabel("Before");
        JLabel labelAfter = new JLabel("After");
        labelBefore.setIcon(iconBefore);
        labelAfter.setIcon(iconAfter);

        JFrame frame = new JFrame();
        frame.setLayout(new FlowLayout());
        frame.setSize(500, 500);
        frame.add(labelBefore);
        frame.add(labelAfter);
        frame.pack();

        frame.setVisible(true);
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
    }
}
