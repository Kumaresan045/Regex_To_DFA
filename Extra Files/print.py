import matplotlib.pyplot as plt
import matplotlib.image as mpimg

def display():
    # Load the SVG file
    img = mpimg.imread('dfa.png')

    # Display the image
    plt.imshow(img)
    plt.axis('off')  # Turn off axis numbers and ticks
    plt.show()
