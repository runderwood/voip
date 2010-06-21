/*
 * Copyright 2002-2008 The Gong Project (http://gong.ust.hk)
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
package gong.ui.plaf;

import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Rectangle;
import javax.swing.JComponent;
import javax.swing.JSlider;
import javax.swing.plaf.basic.BasicSliderUI;

/**
 * This is the UI of the nanogong recording amplitude display.
 * @author Gibson Lam
 * @version 3.0, 08/08/2008
 */
public class NanoAmplitudeUI extends BasicSliderUI {
    
    /**
     * Creates a new instance of NanoAmplitudeUI
     * @param s the slider component
     */
    public NanoAmplitudeUI(JSlider s) {
        super(s);
    }
    
    /**
     * Installs the UI for the component
     * @param c the slider component
     */
    public void installUI(JComponent c) {
        super.installUI(c);
        c.setOpaque(false);
        c.setFocusable(false);
        c.setEnabled(false);
    }
    
    /**
     * Paints the track (amplitude) of the slider
     * @param g the graphics object
     */
    public void paintTrack(Graphics g) {
        Graphics2D g2d = (Graphics2D) g;
        
        Rectangle r = slider.getBounds();
        
        g2d.setColor(slider.getForeground());
        
        float ratio = (float) (slider.getValue() - slider.getMinimum()) / (float) (slider.getMaximum() - slider.getMinimum());
        int max = (int) (r.height * (1f - ratio));
        
        for (int y = r.height - 2; y >= max; y -= 3)
            g2d.fillRect(0, y, r.width, 2);
    }
    
    /**
     * Paints the thumb of the slider (do not paint anything in this case)
     * @param g the graphics object
     */
    public void paintThumb(Graphics g) {
    }
    
}
