import urllib.parse
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common.by import By
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
from selenium.webdriver.common.keys import Keys
import time
import cairosvg

def url_encode(str):
    return urllib.parse.quote(str)

def setup():
    chrome_options = Options()
    chrome_options.add_argument('--headless')  # Run in headless mode
    chrome_options.add_argument('--disable-gpu')  # Disable GPU for headless mode
    chrome_options.add_argument('--no-sandbox')  # Bypass OS security model
    chrome_options.add_argument('--window-size=1920x1080')
    driver = webdriver.Chrome(options=chrome_options)
    return driver

def get_graph_out():
    with open('out.txt','r') as f:
        return f.read()

def get_svg_from_graphviz():
    driver = setup()
    str = get_graph_out()
    str = url_encode(str)
    url = f"https://dreampuf.github.io/GraphvizOnline/#{str}"

    driver.get(url)

    show_raw = WebDriverWait(driver, 10).until(
        EC.element_to_be_clickable((By.XPATH, "/html/body/div[1]/label[3]/input"))
    )

    show_raw.click()

    time.sleep(2)

    svg_text = WebDriverWait(driver, 10).until(
        EC.presence_of_element_located((By.XPATH, "/html/body/div[2]/div[2]"))
    )

    return svg_text.text

def generate_png(svg_text):
    cairosvg.svg2png(bytestring=svg_text.encode('utf-8'), write_to="dfa.png")


#--------------- MAIN ---------------#
if __name__ == "__main__":
    svg_text = get_svg_from_graphviz()
    generate_png(svg_text)