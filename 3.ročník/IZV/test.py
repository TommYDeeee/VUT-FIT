import requests, requests_cache, sqlite3, random, datetime
from bs4 import BeautifulSoup

from decimal import Decimal
sqlite3.register_adapter(Decimal, lambda d: str(d))
sqlite3.register_converter("DECIMAL", lambda s: Decimal(s.decode('ascii')))

def sync(dbfile='data.sqlite'):
    with sqlite3.connect(dbfile, detect_types=sqlite3.PARSE_DECLTYPES) as conn:
        c = conn.cursor()
        c.execute('''CREATE TABLE IF NOT EXISTS products
                  (id TEXT, url TEXT, title TEXT, price DECIMAL, stock_state TEXT, last_update DATETIME, PRIMARY KEY(id,url))''')
        
        c.execute('''CREATE TABLE IF NOT EXISTS products_history
                  (id TEXT, url TEXT, url_alt TEXT, title TEXT, price DECIMAL, stock_state TEXT, last_update DATETIME)''')

        c.execute('''CREATE INDEX IF NOT EXISTS idx_id ON products (id)''')
        c.execute('''CREATE INDEX IF NOT EXISTS idx_idurl ON products_history (id, url)''')

        s = requests.session()

resp = requests.post('https://www.alza.cz/Services/EShopService.svc/Filter', json={
         'idCategory': 18863907, 
         'producers': '', 
         'parameters': [], 
         'idPrefix': 0, 
         'prefixType': 0, 
         'page': 1, 
         'pageTo': 4, 
         'inStock': False, 
         'newsOnly': False, 
         'commodityStatusType': None, 
         'upperDescriptionStatus': 0, 
         'branchId': -2, 
         'sort': 0, 
         'categoryType': 1, 
         'searchTerm': '', 
         'sendProducers': False, 
         'layout': 0, 
         'append': False, 
         'leasingCatId': None, 
         'yearFrom': None, 
         'yearTo': None, 
         'artistId': None, 
         'minPrice': -1, 
         'maxPrice': -1, 
         'shouldDisplayVirtooal': False, 
         'callFromParametrizationDialog': False, 
         'commodityWearType': None, 
         'scroll': 10905, 
         'hash': '#f&cst=null&cud=0&pg=1-4&prod=', 
         'counter': 3
        })

data = resp.json()
data_html = data['d']['Boxes']
beautiful_html = BeautifulSoup(data_html, 'html.parser')
elements =  beautiful_html.find_all('a', {'class':'name browsinglink'})
for element in elements:
    print(element.get("data-impression-id"))
    print(element.get("href"))
    print(element.get("data-impression-name"))
    print(element.get("data-impression-metric2"))
    print(element.get("data-impression-dimension13"))