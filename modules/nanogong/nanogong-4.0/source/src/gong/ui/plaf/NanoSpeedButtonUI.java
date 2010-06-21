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

import java.awt.Color;
import java.awt.Graphics2D;
import java.awt.Polygon;
import java.awt.Rectangle;
import javax.swing.ButtonModel;
import javax.swing.JComponent;

/**
 * This is the UI of the NanoGong speed up/slow down buttons.
 * @author Gibson Lam
 * @version 1.0, 02/09/2007
 */
public class NanoSpeedButtonUI extends NanoButtonUI {
    
    /**
     * The constant for the slow down button
     */
    public static final String SLOW = "slow";
    /**
     * The constant for the speed up button
     */
    public static final String FAST = "fast";
    
    /**
     * Creates an instance of the NanoSpeedButtonUI
     * @param type the icon type of the button
     */
    public NanoSpeedButtonUI(String type) {
        super(type);
    }
    
    /**
     * Paints the icon of the button
     * @param g2d the graphics2d object
     * @param c the button component
     * @param m the button model
     */
    protected void paintIcon(Graphics2D g2d, JComponent c, ButtonModel m) {
        Rectangle r = c.getBounds();
        
        if (m.isEnabled())
            g2d.setColor(new Color(64, 64, 64));
        else
            g2d.setColor(new Color(128, 128, 128));
        
        int mx = r.width / 2 + 1;
        int my = r.height / 2 + 1;
        
        Polygon p = new Polygon();
        
        if (iconType.equals(FAST)) {
            p.addPoint(mx + 4, my);
            p.addPoint(mx - 1, my - 3);
            p.addPoint(mx - 1, my + 2);
            g2d.fillPolygon(p);
            
            g2d.fillRect(mx - 3, my - 3, 1, 5);
        } else if (iconType.equals(SLOW)) {
            p.addPoint(mx - 5, my);
            p.addPoint(mx, my - 3);
            p.addPoint(mx, my + 2);
            g2d.fillPolygon(p);
            
            g2d.fillRect(mx + 1, my - 3, 1, 5);
        }
    }
    
}
