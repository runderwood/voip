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
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Rectangle;
import java.awt.event.MouseEvent;
import javax.swing.JComponent;
import javax.swing.JSlider;
import javax.swing.plaf.basic.BasicSliderUI;

/**
 * This is the UI of the NanoGong timeline.
 * @author Gibson Lam
 * @version 1.0, 02/09/2007
 */
public class NanoTimeUI extends BasicSliderUI {
    
    TimeListener listener;
    
    /**
     * Creates a new instance of NanoTimeUI
     * @param s the slider object
     * @param listener the listener for any time value change
     */
    public NanoTimeUI(JSlider s, TimeListener listener) {
        super(s);
        this.listener = listener;
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
     * Paints the track (the timeline)
     * @param g the graphics object
     */
    public void paintTrack(Graphics g) {
        Graphics2D g2d = (Graphics2D) g;
        
        Rectangle r = slider.getBounds();
        
        g2d.setColor(getShadowColor());
        
        float ratio = (float) (slider.getValue() - slider.getMinimum()) / (float) (slider.getMaximum() - slider.getMinimum());
        int x = (int) Math.ceil((r.width - 1) * ratio);
        
        g2d.setColor(new Color(64, 64, 255));
        g2d.fillRect(0, 0, x, r.height);
        
        g2d.setColor(slider.getBackground().darker());
        g2d.fillRect(x, 0, r.width - x, r.height);
    }
    
    /**
     * Paints the thumb (empty in this case)
     * @param g the graphics object
     */
    public void paintThumb(Graphics g) {
    }
    
    /**
     * Creates the track listener defined in this class
     * @param slider the slider component
     * @return a new track listener
     */
    protected TrackListener createTrackListener(JSlider slider) {
        return new TimeTrackListener();
    }
    
    /**
     * This class defines the extended track listener for event handling of the
     * slider.
     */
    public class TimeTrackListener extends TrackListener {
        protected transient int currentMouseX;
        
        /**
         * The current mouse y location
         */
        protected transient int currentMouseY;
        
        /**
         * Handles the mouse released event
         * @param e the event object
         */
        public void mouseReleased(MouseEvent e) {
            if (!slider.isEnabled()) return;
            
            slider.repaint();
        }
        
        /**
         * Handles the mouse pressed event by calculating the new time value
         * @param e the event object
         */
        public void mousePressed(MouseEvent e) {
            if (!slider.isEnabled()) return;
            
            currentMouseX = e.getX();
            currentMouseY = e.getY();
            
            if (slider.isRequestFocusEnabled()) slider.requestFocus();
            
            Dimension size = slider.getSize();
            
            int value = (int) ((float) currentMouseX / (float) size.width * (slider.getMaximum() - slider.getMinimum())) + slider.getMinimum();
            slider.setValue(value);
            if (listener != null) listener.timeUpdate(value);
        }
        
        /**
         * Handles the mouse dragged event (do nothing)
         * @param e the event object
         */
        public void mouseDragged(MouseEvent e) {}
        
        /**
         * Handles the mouse moved event (do nothing)
         * @param e the event object
         */
        public void mouseMoved(MouseEvent e) {}
        
    }
    
    /**
     * The interface for listening the time value change of the timeline.
     */
    public interface TimeListener {
        
        /**
         * Receives the modified time from the timeline
         * @param value the time value
         */
        public void timeUpdate(int value);
        
    }
    
}
