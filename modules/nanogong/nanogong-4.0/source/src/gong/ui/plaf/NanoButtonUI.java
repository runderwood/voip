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
import java.awt.GradientPaint;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Polygon;
import java.awt.Rectangle;
import java.awt.RenderingHints;
import javax.swing.AbstractButton;
import javax.swing.ButtonModel;
import javax.swing.JComponent;
import javax.swing.plaf.basic.BasicButtonUI;

/**
 * This is the UI of the NanoGong buttons.
 * @author Gibson Lam
 * @version 1.0, 02/09/2007
 */
public class NanoButtonUI extends BasicButtonUI {
    
    /**
     * The constant for the play button
     */
    public static final String PLAY = "play";

    /**
     * The constant for the record button
     */
    public static final String RECORD = "record";

    /**
     * The constant for the pause button
     */
    public static final String PAUSE = "pause";

    /**
     * The constant for the stop button
     */
    public static final String STOP = "stop";

    /**
     * The constant for the save button
     */
    public static final String SAVE = "save";
    
    /**
     * The type of icon for the button
     */
    protected String iconType;
    
    /**
     * Creates an instance of the NanoButtonUI
     * @param type the icon type
     */
    public NanoButtonUI(String type) {
        iconType = type;
    }

    /**
     * Sets the icon type of the button
     * @param type the icon type
     */
    public void setIconType(String type) {
        iconType = type;
    }
    
    /**
     * Installs the UI for the component
     * @param c the button component
     */
    public void installUI(JComponent c) {
        super.installUI(c);
        c.setBorder(null);
        c.setOpaque(false);
    }
    
    /**
     * Paints the button
     * @param g the graphics object
     * @param c the button component
     */
    public void paint(Graphics g, JComponent c) {
        AbstractButton b = (AbstractButton) c;
        ButtonModel m = b.getModel();
        Graphics2D g2d = (Graphics2D) g;
        
        g2d.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
        
        Rectangle r = c.getBounds();
        
        if (m.isEnabled()) {
            if (m.isPressed()) {
                g2d.setColor(Color.BLACK);
                g2d.fillOval(0, 0, r.width, r.height);
                
                g2d.setColor(Color.WHITE);
                g2d.fillOval(2, 2, r.width - 2, r.height - 2);
                
                g2d.setColor(b.getBackground().darker());
                g2d.fillOval(1, 1, r.width - 3, r.height - 3);
                
                g2d.setPaint(new GradientPaint(1, 1, b.getBackground(), r.width - 3, r.height - 3, Color.WHITE));
                g2d.fillOval(2, 2, r.width - 3, r.height - 3);
            } else {
                g2d.setColor(Color.WHITE);
                g2d.fillOval(0, 0, r.width, r.height);
                
                g2d.setColor(Color.BLACK);
                g2d.fillOval(2, 2, r.width - 2, r.height - 2);
                
                g2d.setColor(b.getBackground().darker());
                g2d.fillOval(2, 2, r.width - 3, r.height - 3);
                
                g2d.setPaint(new GradientPaint(1, 1, Color.WHITE, r.width - 3, r.height - 3, b.getBackground()));
                g2d.fillOval(1, 1, r.width - 3, r.height - 3);
            }
        } else {
            g2d.setColor(b.getBackground().darker());
            g2d.fillOval(0, 0, r.width, r.height);
            
            g2d.setColor(b.getBackground());
            g2d.fillOval(1, 1, r.width - 2, r.height - 2);
        }
        
        if (m.isPressed()) g2d.translate(1, 1);
        paintIcon(g2d, c, m);
        if (m.isPressed()) g2d.translate(-1, -1);
        
        g2d.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_OFF);
    }
    
    /**
     * Paints the icon of the button
     * @param g2d the graphics2d object
     * @param c the button component
     * @param m the button model
     */
    protected void paintIcon(Graphics2D g2d, JComponent c, ButtonModel m) {
        Rectangle r = c.getBounds();
        
        if (m.isEnabled()) {
            if (iconType.equals(RECORD))
                g2d.setColor(new Color(192, 0, 0));
            else
                g2d.setColor(new Color(64, 64, 64));
        } else
            g2d.setColor(new Color(128, 128, 128));
        
        // Paint the icon for each type
        if (iconType.equals(PLAY)) {
            int mx = r.width / 2 + 1;
            int my = r.height / 2 + 1;
            
            Polygon p = new Polygon();
            p.addPoint(mx + 6, my);
            p.addPoint(mx - 5, my - 6);
            p.addPoint(mx - 5, my + 5);
            g2d.fillPolygon(p);
        } else if (iconType.equals(RECORD)) {
            g2d.fillOval((r.width - 11) / 2, (r.height - 11) / 2, 11, 11);
        } else if (iconType.equals(PAUSE)) {
            g2d.fillRoundRect((r.width - 11) / 2, (r.height - 11) / 2, 5, 11, 5, 5);
            g2d.fillRoundRect((r.width - 11) / 2 + 6, (r.height - 11) / 2, 5, 11, 5, 5);
        } else if (iconType.equals(STOP)) {
            g2d.fillRoundRect((r.width - 11) / 2, (r.height - 11) / 2, 11, 11, 5, 5);
        } else if (iconType.equals(SAVE)) {
            g2d.fillRoundRect((r.width - 11) / 2, (r.height - 11) / 2, 11, 11, 5, 5);
            
            g2d.setColor(c.getBackground());
            g2d.fillRect((r.width - 11) / 2 + 3, (r.height - 11) / 2 + 7, 5, 4);
        }
    }
    
}
