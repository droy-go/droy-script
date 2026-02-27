// Droy Runtime - Live UI Preview Engine
// ======================================

// Component Templates
const componentTemplates = {
    button: `block: key('myButton') {
    sty {
        set type = 'button'
        set text = 'Click Me'
        set variant = 'primary'
        set onClick = 'handleClick()'
    }
}`,
    
    input: `block: key('myInput') {
    sty {
        set type = 'input'
        set placeholder = 'Enter text...'
        set label = 'Input Field'
    }
}`,
    
    form: `block: key('myForm') {
    sty {
        set type = 'form'
        set title = 'Contact Form'
    }
    
    block: key('nameField') {
        sty {
            set type = 'input'
            set label = 'Name'
            set placeholder = 'Your name'
        }
    }
    
    block: key('emailField') {
        sty {
            set type = 'input'
            set label = 'Email'
            set type = 'email'
            set placeholder = 'your@email.com'
        }
    }
    
    block: key('submitBtn') {
        sty {
            set type = 'button'
            set text = 'Submit'
            set variant = 'primary'
        }
    }
}`,
    
    card: `block: key('myCard') {
    sty {
        set type = 'card'
        set title = 'Card Title'
        set content = 'This is a card component with some content.'
    }
}`,
    
    modal: `block: key('myModal') {
    sty {
        set type = 'modal'
        set title = 'Modal Title'
        set content = 'Modal content goes here...'
        set showClose = true
    }
}`,
    
    navbar: `block: key('myNavbar') {
    sty {
        set type = 'navbar'
        set brand = 'MyApp'
        set items = ['Home', 'About', 'Contact']
    }
}`,
    
    table: `block: key('myTable') {
    sty {
        set type = 'table'
        set headers = ['Name', 'Email', 'Role']
        set data = [
            ['John', 'john@example.com', 'Admin'],
            ['Jane', 'jane@example.com', 'User']
        ]
    }
}`,
    
    tabs: `block: key('myTabs') {
    sty {
        set type = 'tabs'
        set tabs = ['Tab 1', 'Tab 2', 'Tab 3']
        set activeTab = 0
    }
}`,
    
    alert: `block: key('myAlert') {
    sty {
        set type = 'alert'
        set message = 'This is an alert message!'
        set variant = 'info'
    }
}`,
    
    text: `block: key('myText') {
    sty {
        set type = 'text'
        set content = 'Hello, World!'
        set size = 'medium'
    }
}`,
    
    image: `block: key('myImage') {
    sty {
        set type = 'image'
        set src = 'https://via.placeholder.com/300x200'
        set alt = 'Placeholder'
    }
}`,
    
    checkbox: `block: key('myCheckbox') {
    sty {
        set type = 'checkbox'
        set label = 'Check me'
        set checked = false
    }
}`,
    
    radio: `block: key('myRadio') {
    sty {
        set type = 'radio'
        set label = 'Option 1'
        set name = 'options'
        set value = '1'
    }
}`,
    
    select: `block: key('mySelect') {
    sty {
        set type = 'select'
        set label = 'Choose option'
        set options = ['Option 1', 'Option 2', 'Option 3']
    }
}`,
    
    textarea: `block: key('myTextarea') {
    sty {
        set type = 'textarea'
        set label = 'Description'
        set placeholder = 'Enter description...'
        set rows = 4
    }
}`,
    
    container: `block: key('myContainer') {
    sty {
        set type = 'container'
        set layout = 'flex'
        set gap = '10px'
    }
}`,
    
    row: `block: key('myRow') {
    sty {
        set type = 'row'
        set columns = 2
    }
}`,
    
    column: `block: key('myColumn') {
    sty {
        set type = 'column'
        set width = '50%'
    }
}`,
    
    divider: `block: key('myDivider') {
    sty {
        set type = 'divider'
    }
}`,
    
    breadcrumb: `block: key('myBreadcrumb') {
    sty {
        set type = 'breadcrumb'
        set items = ['Home', 'Products', 'Details']
    }
}`,
    
    pagination: `block: key('myPagination') {
    sty {
        set type = 'pagination'
        set total = 10
        set current = 1
    }
}`,
    
    progress: `block: key('myProgress') {
    sty {
        set type = 'progress'
        set value = 75
        set max = 100
    }
}`,
    
    toast: `block: key('myToast') {
    sty {
        set type = 'toast'
        set message = 'Operation successful!'
        set duration = 3000
    }
}`,
    
    badge: `block: key('myBadge') {
    sty {
        set type = 'badge'
        set text = 'New'
        set variant = 'primary'
    }
}`,
    
    list: `block: key('myList') {
    sty {
        set type = 'list'
        set items = ['Item 1', 'Item 2', 'Item 3']
    }
}`,
    
    timeline: `block: key('myTimeline') {
    sty {
        set type = 'timeline'
        set events = [
            { time: '2024-01-01', title: 'Event 1' },
            { time: '2024-02-01', title: 'Event 2' }
        ]
    }
}`,
    
    chart: `block: key('myChart') {
    sty {
        set type = 'chart'
        set chartType = 'bar'
        set data = [10, 20, 30, 40, 50]
        set labels = ['A', 'B', 'C', 'D', 'E']
    }
}`,
    
    icon: `block: key('myIcon') {
    sty {
        set type = 'icon'
        set name = 'star'
        set size = 'medium'
    }
}`
};

// Component Properties
const componentProperties = {
    button: {
        basic: [
            { name: 'text', type: 'text', label: 'Button Text', default: 'Click Me' },
            { name: 'variant', type: 'select', label: 'Variant', options: ['primary', 'secondary', 'success', 'danger'], default: 'primary' },
            { name: 'size', type: 'select', label: 'Size', options: ['small', 'medium', 'large'], default: 'medium' },
            { name: 'disabled', type: 'checkbox', label: 'Disabled', default: false }
        ],
        events: [
            { name: 'onClick', label: 'On Click' },
            { name: 'onHover', label: 'On Hover' }
        ]
    },
    
    input: {
        basic: [
            { name: 'label', type: 'text', label: 'Label', default: '' },
            { name: 'placeholder', type: 'text', label: 'Placeholder', default: 'Enter text...' },
            { name: 'type', type: 'select', label: 'Input Type', options: ['text', 'email', 'password', 'number', 'tel'], default: 'text' },
            { name: 'required', type: 'checkbox', label: 'Required', default: false }
        ],
        events: [
            { name: 'onChange', label: 'On Change' },
            { name: 'onFocus', label: 'On Focus' },
            { name: 'onBlur', label: 'On Blur' }
        ]
    },
    
    card: {
        basic: [
            { name: 'title', type: 'text', label: 'Title', default: 'Card Title' },
            { name: 'content', type: 'textarea', label: 'Content', default: 'Card content goes here...' },
            { name: 'image', type: 'text', label: 'Image URL', default: '' },
            { name: 'footer', type: 'text', label: 'Footer', default: '' }
        ],
        events: []
    },
    
    alert: {
        basic: [
            { name: 'message', type: 'textarea', label: 'Message', default: 'Alert message' },
            { name: 'variant', type: 'select', label: 'Variant', options: ['info', 'success', 'warning', 'error'], default: 'info' },
            { name: 'dismissible', type: 'checkbox', label: 'Dismissible', default: true }
        ],
        events: [
            { name: 'onClose', label: 'On Close' }
        ]
    },
    
    text: {
        basic: [
            { name: 'content', type: 'textarea', label: 'Content', default: 'Hello, World!' },
            { name: 'size', type: 'select', label: 'Size', options: ['small', 'medium', 'large', 'xlarge'], default: 'medium' },
            { name: 'weight', type: 'select', label: 'Weight', options: ['normal', 'bold', 'light'], default: 'normal' },
            { name: 'color', type: 'text', label: 'Color', default: '' }
        ],
        events: []
    },
    
    image: {
        basic: [
            { name: 'src', type: 'text', label: 'Image URL', default: 'https://via.placeholder.com/300x200' },
            { name: 'alt', type: 'text', label: 'Alt Text', default: 'Image' },
            { name: 'width', type: 'text', label: 'Width', default: '' },
            { name: 'height', type: 'text', label: 'Height', default: '' }
        ],
        events: [
            { name: 'onClick', label: 'On Click' },
            { name: 'onLoad', label: 'On Load' }
        ]
    },
    
    progress: {
        basic: [
            { name: 'value', type: 'number', label: 'Value', default: 50 },
            { name: 'max', type: 'number', label: 'Maximum', default: 100 },
            { name: 'showLabel', type: 'checkbox', label: 'Show Label', default: true }
        ],
        events: []
    },
    
    badge: {
        basic: [
            { name: 'text', type: 'text', label: 'Text', default: 'New' },
            { name: 'variant', type: 'select', label: 'Variant', options: ['primary', 'secondary', 'success', 'danger', 'warning'], default: 'primary' }
        ],
        events: []
    },
    
    checkbox: {
        basic: [
            { name: 'label', type: 'text', label: 'Label', default: 'Check me' },
            { name: 'checked', type: 'checkbox', label: 'Checked', default: false }
        ],
        events: [
            { name: 'onChange', label: 'On Change' }
        ]
    },
    
    select: {
        basic: [
            { name: 'label', type: 'text', label: 'Label', default: 'Select option' },
            { name: 'options', type: 'textarea', label: 'Options (one per line)', default: 'Option 1\nOption 2\nOption 3' },
            { name: 'multiple', type: 'checkbox', label: 'Multiple', default: false }
        ],
        events: [
            { name: 'onChange', label: 'On Change' }
        ]
    },
    
    textarea: {
        basic: [
            { name: 'label', type: 'text', label: 'Label', default: 'Description' },
            { name: 'placeholder', type: 'text', label: 'Placeholder', default: 'Enter text...' },
            { name: 'rows', type: 'number', label: 'Rows', default: 4 }
        ],
        events: [
            { name: 'onChange', label: 'On Change' }
        ]
    },
    
    list: {
        basic: [
            { name: 'items', type: 'textarea', label: 'Items (one per line)', default: 'Item 1\nItem 2\nItem 3' },
            { name: 'ordered', type: 'checkbox', label: 'Ordered List', default: false }
        ],
        events: [
            { name: 'onItemClick', label: 'On Item Click' }
        ]
    },
    
    divider: {
        basic: [
            { name: 'text', type: 'text', label: 'Text', default: '' },
            { name: 'orientation', type: 'select', label: 'Orientation', options: ['horizontal', 'vertical'], default: 'horizontal' }
        ],
        events: []
    },
    
    icon: {
        basic: [
            { name: 'name', type: 'text', label: 'Icon Name', default: 'star' },
            { name: 'size', type: 'select', label: 'Size', options: ['small', 'medium', 'large'], default: 'medium' },
            { name: 'color', type: 'text', label: 'Color', default: '' }
        ],
        events: [
            { name: 'onClick', label: 'On Click' }
        ]
    }
};

// Droy Parser
class DroyParser {
    constructor() {
        this.elements = [];
    }
    
    parse(code) {
        this.elements = [];
        const lines = code.split('\n');
        let currentBlock = null;
        let inStyBlock = false;
        let styProperties = {};
        
        for (let i = 0; i < lines.length; i++) {
            const line = lines[i].trim();
            
            // Skip empty lines and comments
            if (!line || line.startsWith('//')) continue;
            
            // Block start
            if (line.startsWith('block:')) {
                const keyMatch = line.match(/key\(['"](.+?)['"]\)/);
                if (keyMatch) {
                    currentBlock = {
                        type: 'block',
                        key: keyMatch[1],
                        properties: {}
                    };
                }
            }
            // Sty block start
            else if (line.startsWith('sty {')) {
                inStyBlock = true;
                styProperties = {};
            }
            // Sty block end
            else if (line === '}' && inStyBlock) {
                inStyBlock = false;
                if (currentBlock) {
                    currentBlock.properties = { ...styProperties };
                    this.elements.push(currentBlock);
                    currentBlock = null;
                }
            }
            // Property inside sty block
            else if (inStyBlock && line.includes('=')) {
                const match = line.match(/set\s+(\w+)\s*=\s*(.+)/);
                if (match) {
                    const [, key, value] = match;
                    styProperties[key] = this.parseValue(value);
                }
            }
            // Block end
            else if (line === '}' && currentBlock && !inStyBlock) {
                this.elements.push(currentBlock);
                currentBlock = null;
            }
        }
        
        return this.elements;
    }
    
    parseValue(value) {
        value = value.trim();
        
        // String
        if ((value.startsWith('"') && value.endsWith('"')) || 
            (value.startsWith("'") && value.endsWith("'"))) {
            return value.slice(1, -1);
        }
        
        // Number
        if (!isNaN(value)) {
            return Number(value);
        }
        
        // Boolean
        if (value === 'true') return true;
        if (value === 'false') return false;
        
        // Array
        if (value.startsWith('[') && value.endsWith(']')) {
            try {
                return JSON.parse(value.replace(/'/g, '"'));
            } catch {
                return value;
            }
        }
        
        return value;
    }
}

// UI Renderer
class UIRenderer {
    constructor(container) {
        this.container = container;
    }
    
    render(elements) {
        this.container.innerHTML = '';
        
        elements.forEach(element => {
            const el = this.createElement(element);
            if (el) {
                this.container.appendChild(el);
            }
        });
    }
    
    createElement(element) {
        const type = element.properties.type || 'div';
        const wrapper = document.createElement('div');
        wrapper.className = 'droy-element';
        wrapper.dataset.key = element.key;
        
        let el;
        
        switch (type) {
            case 'button':
                el = this.createButton(element.properties);
                break;
            case 'input':
                el = this.createInput(element.properties);
                break;
            case 'card':
                el = this.createCard(element.properties);
                break;
            case 'alert':
                el = this.createAlert(element.properties);
                break;
            case 'text':
                el = this.createText(element.properties);
                break;
            case 'image':
                el = this.createImage(element.properties);
                break;
            case 'progress':
                el = this.createProgress(element.properties);
                break;
            case 'badge':
                el = this.createBadge(element.properties);
                break;
            case 'checkbox':
                el = this.createCheckbox(element.properties);
                break;
            case 'select':
                el = this.createSelect(element.properties);
                break;
            case 'textarea':
                el = this.createTextarea(element.properties);
                break;
            case 'list':
                el = this.createList(element.properties);
                break;
            case 'divider':
                el = this.createDivider(element.properties);
                break;
            case 'icon':
                el = this.createIcon(element.properties);
                break;
            default:
                el = document.createElement('div');
                el.textContent = `Unknown type: ${type}`;
        }
        
        if (el) {
            wrapper.appendChild(el);
        }
        
        return wrapper;
    }
    
    createButton(props) {
        const btn = document.createElement('button');
        btn.className = `droy-button ${props.variant || 'primary'}`;
        btn.textContent = props.text || 'Button';
        
        if (props.size) {
            btn.style.padding = props.size === 'small' ? '6px 12px' : 
                               props.size === 'large' ? '14px 28px' : '10px 20px';
        }
        
        if (props.disabled) {
            btn.disabled = true;
            btn.style.opacity = '0.5';
        }
        
        return btn;
    }
    
    createInput(props) {
        const wrapper = document.createElement('div');
        wrapper.style.marginBottom = '15px';
        
        if (props.label) {
            const label = document.createElement('label');
            label.textContent = props.label;
            label.style.display = 'block';
            label.style.marginBottom = '5px';
            label.style.fontSize = '14px';
            label.style.color = '#334155';
            wrapper.appendChild(label);
        }
        
        const input = document.createElement('input');
        input.type = props.type || 'text';
        input.className = 'droy-input';
        input.placeholder = props.placeholder || '';
        
        if (props.required) {
            input.required = true;
        }
        
        wrapper.appendChild(input);
        return wrapper;
    }
    
    createCard(props) {
        const card = document.createElement('div');
        card.className = 'droy-card';
        
        if (props.image) {
            const img = document.createElement('img');
            img.src = props.image;
            img.style.width = '100%';
            img.style.borderRadius = '8px 8px 0 0';
            img.style.marginBottom = '15px';
            card.appendChild(img);
        }
        
        if (props.title) {
            const title = document.createElement('div');
            title.className = 'droy-card-header';
            title.textContent = props.title;
            card.appendChild(title);
        }
        
        if (props.content) {
            const content = document.createElement('div');
            content.className = 'droy-card-body';
            content.textContent = props.content;
            card.appendChild(content);
        }
        
        if (props.footer) {
            const footer = document.createElement('div');
            footer.style.marginTop = '15px';
            footer.style.paddingTop = '15px';
            footer.style.borderTop = '1px solid #e2e8f0';
            footer.style.color = '#64748b';
            footer.style.fontSize = '14px';
            footer.textContent = props.footer;
            card.appendChild(footer);
        }
        
        return card;
    }
    
    createAlert(props) {
        const alert = document.createElement('div');
        alert.className = `droy-alert ${props.variant || 'info'}`;
        alert.textContent = props.message || 'Alert message';
        
        if (props.dismissible) {
            const closeBtn = document.createElement('button');
            closeBtn.innerHTML = '&times;';
            closeBtn.style.cssText = `
                float: right;
                background: none;
                border: none;
                font-size: 20px;
                cursor: pointer;
                opacity: 0.5;
            `;
            closeBtn.onclick = () => alert.remove();
            alert.appendChild(closeBtn);
        }
        
        return alert;
    }
    
    createText(props) {
        const text = document.createElement('p');
        text.textContent = props.content || '';
        
        const sizes = {
            small: '14px',
            medium: '16px',
            large: '20px',
            xlarge: '24px'
        };
        
        text.style.fontSize = sizes[props.size] || sizes.medium;
        text.style.fontWeight = props.weight || 'normal';
        
        if (props.color) {
            text.style.color = props.color;
        }
        
        return text;
    }
    
    createImage(props) {
        const img = document.createElement('img');
        img.src = props.src || '';
        img.alt = props.alt || '';
        
        if (props.width) img.style.width = props.width;
        if (props.height) img.style.height = props.height;
        
        img.style.maxWidth = '100%';
        img.style.borderRadius = '8px';
        
        return img;
    }
    
    createProgress(props) {
        const wrapper = document.createElement('div');
        wrapper.style.width = '100%';
        
        const progress = document.createElement('div');
        progress.style.cssText = `
            width: 100%;
            height: 20px;
            background: #e2e8f0;
            border-radius: 10px;
            overflow: hidden;
        `;
        
        const bar = document.createElement('div');
        const value = props.value || 0;
        const max = props.max || 100;
        const percentage = (value / max) * 100;
        
        bar.style.cssText = `
            width: ${percentage}%;
            height: 100%;
            background: linear-gradient(90deg, #6366f1, #8b5cf6);
            transition: width 0.3s ease;
        `;
        
        progress.appendChild(bar);
        wrapper.appendChild(progress);
        
        if (props.showLabel) {
            const label = document.createElement('div');
            label.textContent = `${Math.round(percentage)}%`;
            label.style.textAlign = 'center';
            label.style.marginTop = '5px';
            label.style.fontSize = '14px';
            label.style.color = '#64748b';
            wrapper.appendChild(label);
        }
        
        return wrapper;
    }
    
    createBadge(props) {
        const badge = document.createElement('span');
        badge.textContent = props.text || '';
        
        const colors = {
            primary: '#6366f1',
            secondary: '#8b5cf6',
            success: '#22c55e',
            danger: '#ef4444',
            warning: '#f59e0b'
        };
        
        badge.style.cssText = `
            display: inline-block;
            padding: 4px 12px;
            border-radius: 20px;
            font-size: 12px;
            font-weight: 600;
            color: white;
            background: ${colors[props.variant] || colors.primary};
        `;
        
        return badge;
    }
    
    createCheckbox(props) {
        const wrapper = document.createElement('label');
        wrapper.style.cssText = `
            display: flex;
            align-items: center;
            gap: 8px;
            cursor: pointer;
        `;
        
        const checkbox = document.createElement('input');
        checkbox.type = 'checkbox';
        checkbox.checked = props.checked || false;
        
        const label = document.createElement('span');
        label.textContent = props.label || '';
        label.style.fontSize = '14px';
        
        wrapper.appendChild(checkbox);
        wrapper.appendChild(label);
        
        return wrapper;
    }
    
    createSelect(props) {
        const wrapper = document.createElement('div');
        wrapper.style.marginBottom = '15px';
        
        if (props.label) {
            const label = document.createElement('label');
            label.textContent = props.label;
            label.style.display = 'block';
            label.style.marginBottom = '5px';
            label.style.fontSize = '14px';
            wrapper.appendChild(label);
        }
        
        const select = document.createElement('select');
        select.className = 'droy-input';
        select.multiple = props.multiple || false;
        
        const options = props.options || [];
        if (typeof options === 'string') {
            options.split('\n').forEach(opt => {
                const option = document.createElement('option');
                option.textContent = opt.trim();
                select.appendChild(option);
            });
        } else if (Array.isArray(options)) {
            options.forEach(opt => {
                const option = document.createElement('option');
                option.textContent = opt;
                select.appendChild(option);
            });
        }
        
        wrapper.appendChild(select);
        return wrapper;
    }
    
    createTextarea(props) {
        const wrapper = document.createElement('div');
        wrapper.style.marginBottom = '15px';
        
        if (props.label) {
            const label = document.createElement('label');
            label.textContent = props.label;
            label.style.display = 'block';
            label.style.marginBottom = '5px';
            label.style.fontSize = '14px';
            wrapper.appendChild(label);
        }
        
        const textarea = document.createElement('textarea');
        textarea.className = 'droy-input';
        textarea.placeholder = props.placeholder || '';
        textarea.rows = props.rows || 4;
        
        wrapper.appendChild(textarea);
        return wrapper;
    }
    
    createList(props) {
        const list = document.createElement(props.ordered ? 'ol' : 'ul');
        list.style.paddingLeft = '20px';
        
        const items = props.items || [];
        if (typeof items === 'string') {
            items.split('\n').forEach(item => {
                const li = document.createElement('li');
                li.textContent = item.trim();
                li.style.marginBottom = '5px';
                list.appendChild(li);
            });
        } else if (Array.isArray(items)) {
            items.forEach(item => {
                const li = document.createElement('li');
                li.textContent = item;
                li.style.marginBottom = '5px';
                list.appendChild(li);
            });
        }
        
        return list;
    }
    
    createDivider(props) {
        const divider = document.createElement('div');
        
        if (props.text) {
            divider.style.cssText = `
                display: flex;
                align-items: center;
                text-align: center;
                margin: 20px 0;
            `;
            divider.innerHTML = `
                <span style="flex: 1; height: 1px; background: #e2e8f0;"></span>
                <span style="padding: 0 15px; color: #64748b; font-size: 14px;">${props.text}</span>
                <span style="flex: 1; height: 1px; background: #e2e8f0;"></span>
            `;
        } else {
            divider.style.cssText = `
                height: 1px;
                background: #e2e8f0;
                margin: 20px 0;
            `;
        }
        
        return divider;
    }
    
    createIcon(props) {
        const icon = document.createElement('i');
        icon.className = `fas fa-${props.name || 'star'}`;
        
        const sizes = {
            small: '16px',
            medium: '24px',
            large: '32px'
        };
        
        icon.style.fontSize = sizes[props.size] || sizes.medium;
        
        if (props.color) {
            icon.style.color = props.color;
        }
        
        return icon;
    }
}

// Main Runtime Class
class DroyRuntime {
    constructor() {
        this.parser = new DroyParser();
        this.renderer = new UIRenderer(document.getElementById('preview-content'));
        this.selectedElement = null;
        this.elements = [];
        
        this.init();
    }
    
    init() {
        this.initEventListeners();
        this.initDragAndDrop();
        this.initViewModes();
    }
    
    initEventListeners() {
        // Template selector
        document.getElementById('code-template').addEventListener('change', (e) => {
            if (e.target.value && componentTemplates[e.target.value]) {
                document.getElementById('droy-code').value = componentTemplates[e.target.value];
                this.execute();
            }
        });
        
        // Execute button
        document.getElementById('btn-execute').addEventListener('click', () => {
            this.execute();
        });
        
        // Reload button
        document.getElementById('btn-reload').addEventListener('click', () => {
            this.execute();
        });
        
        // Clear button
        document.getElementById('btn-clear').addEventListener('click', () => {
            document.getElementById('droy-code').value = '';
            document.getElementById('preview-content').innerHTML = `
                <div class="empty-state">
                    <i class="fas fa-cube"></i>
                    <p>Enter Droy code to see live preview</p>
                </div>
            `;
            this.updateGeneratedCode('');
        });
        
        // Code input change
        document.getElementById('droy-code').addEventListener('input', () => {
            this.execute();
        });
        
        // Component search
        document.getElementById('component-search').addEventListener('input', (e) => {
            this.filterComponents(e.target.value);
        });
        
        // Panel tabs
        document.querySelectorAll('.panel-tab').forEach(tab => {
            tab.addEventListener('click', () => {
                this.switchTab(tab.dataset.tab);
            });
        });
        
        // Component items click
        document.querySelectorAll('.component-item').forEach(item => {
            item.addEventListener('click', () => {
                const component = item.dataset.component;
                if (componentTemplates[component]) {
                    document.getElementById('droy-code').value = componentTemplates[component];
                    this.execute();
                }
            });
        });
    }
    
    initDragAndDrop() {
        const componentItems = document.querySelectorAll('.component-item');
        const codeTextarea = document.getElementById('droy-code');
        
        componentItems.forEach(item => {
            item.addEventListener('dragstart', (e) => {
                e.dataTransfer.setData('component', item.dataset.component);
            });
        });
        
        codeTextarea.addEventListener('dragover', (e) => {
            e.preventDefault();
        });
        
        codeTextarea.addEventListener('drop', (e) => {
            e.preventDefault();
            const component = e.dataTransfer.getData('component');
            if (componentTemplates[component]) {
                const currentCode = codeTextarea.value;
                const newCode = currentCode + '\n\n' + componentTemplates[component];
                codeTextarea.value = newCode;
                this.execute();
            }
        });
    }
    
    initViewModes() {
        document.querySelectorAll('.view-btn').forEach(btn => {
            btn.addEventListener('click', () => {
                document.querySelectorAll('.view-btn').forEach(b => b.classList.remove('active'));
                btn.classList.add('active');
                
                const frame = document.getElementById('preview-frame');
                frame.className = `preview-frame ${btn.dataset.view}`;
            });
        });
    }
    
    execute() {
        const code = document.getElementById('droy-code').value;
        
        if (!code.trim()) {
            return;
        }
        
        try {
            this.elements = this.parser.parse(code);
            this.renderer.render(this.elements);
            this.updateGeneratedCode(code);
            this.attachElementListeners();
        } catch (error) {
            console.error('Parse error:', error);
        }
    }
    
    attachElementListeners() {
        document.querySelectorAll('.droy-element').forEach(el => {
            el.addEventListener('click', (e) => {
                e.stopPropagation();
                this.selectElement(el);
            });
        });
    }
    
    selectElement(el) {
        document.querySelectorAll('.droy-element').forEach(e => e.classList.remove('selected'));
        el.classList.add('selected');
        this.selectedElement = el;
        
        const key = el.dataset.key;
        const element = this.elements.find(e => e.key === key);
        
        if (element) {
            this.showProperties(element);
            this.showEvents(element);
            this.showStyles(element);
        }
    }
    
    showProperties(element) {
        const type = element.properties.type || 'div';
        const props = componentProperties[type] || { basic: [] };
        const grid = document.getElementById('properties-grid');
        
        let html = '<div class="property-group"><h5>Basic Properties</h5>';
        
        props.basic.forEach(prop => {
            const value = element.properties[prop.name] !== undefined 
                ? element.properties[prop.name] 
                : prop.default;
            
            html += `<div class="property-field">`;
            html += `<label>${prop.label}</label>`;
            
            if (prop.type === 'text') {
                html += `<input type="text" value="${value || ''}" data-prop="${prop.name}">`;
            } else if (prop.type === 'textarea') {
                html += `<textarea rows="3" data-prop="${prop.name}">${value || ''}</textarea>`;
            } else if (prop.type === 'number') {
                html += `<input type="number" value="${value}" data-prop="${prop.name}">`;
            } else if (prop.type === 'select') {
                html += `<select data-prop="${prop.name}">`;
                prop.options.forEach(opt => {
                    html += `<option value="${opt}" ${opt === value ? 'selected' : ''}>${opt}</option>`;
                });
                html += `</select>`;
            } else if (prop.type === 'checkbox') {
                html += `<input type="checkbox" ${value ? 'checked' : ''} data-prop="${prop.name}">`;
            }
            
            html += `</div>`;
        });
        
        html += '</div>';
        grid.innerHTML = html;
        
        // Add change listeners
        grid.querySelectorAll('input, select, textarea').forEach(input => {
            input.addEventListener('change', (e) => {
                const propName = e.target.dataset.prop;
                let propValue = e.target.value;
                
                if (e.target.type === 'checkbox') {
                    propValue = e.target.checked;
                } else if (e.target.type === 'number') {
                    propValue = Number(propValue);
                }
                
                element.properties[propName] = propValue;
                this.renderer.render(this.elements);
                this.updateGeneratedCodeFromElements();
            });
        });
    }
    
    showEvents(element) {
        const type = element.properties.type || 'div';
        const props = componentProperties[type] || { events: [] };
        const list = document.getElementById('events-list');
        
        if (props.events.length === 0) {
            list.innerHTML = '<p class="empty-message">No events available for this component</p>';
            return;
        }
        
        let html = '<div class="property-group"><h5>Event Handlers</h5>';
        
        props.events.forEach(event => {
            html += `<div class="property-field">`;
            html += `<label>${event.label}</label>`;
            html += `<input type="text" placeholder="functionName()" data-event="${event.name}">`;
            html += `</div>`;
        });
        
        html += '</div>';
        list.innerHTML = html;
    }
    
    showStyles(element) {
        const editor = document.getElementById('styles-editor');
        
        editor.innerHTML = `
            <div class="property-group">
                <h5>Custom CSS</h5>
                <div class="property-field">
                    <textarea rows="10" placeholder="/* Enter custom CSS */
.my-element {
    background: #f0f0f0;
    padding: 20px;
}"></textarea>
                </div>
            </div>
        `;
    }
    
    updateGeneratedCode(code) {
        const pre = document.getElementById('generated-code');
        pre.innerHTML = `<code>${this.escapeHtml(code)}</code>`;
    }
    
    updateGeneratedCodeFromElements() {
        let code = '';
        
        this.elements.forEach(el => {
            code += `block: key('${el.key}') {\n`;
            code += `    sty {\n`;
            
            Object.entries(el.properties).forEach(([key, value]) => {
                if (typeof value === 'string') {
                    code += `        set ${key} = "${value}"\n`;
                } else if (typeof value === 'boolean') {
                    code += `        set ${key} = ${value}\n`;
                } else if (Array.isArray(value)) {
                    code += `        set ${key} = [${value.map(v => `"${v}"`).join(', ')}]\n`;
                } else {
                    code += `        set ${key} = ${value}\n`;
                }
            });
            
            code += `    }\n`;
            code += `}\n\n`;
        });
        
        this.updateGeneratedCode(code);
    }
    
    switchTab(tabName) {
        document.querySelectorAll('.panel-tab').forEach(t => t.classList.remove('active'));
        document.querySelector(`.panel-tab[data-tab="${tabName}"]`).classList.add('active');
        
        document.querySelectorAll('.tab-content').forEach(c => c.classList.remove('active'));
        document.getElementById(`${tabName}-tab`).classList.add('active');
    }
    
    filterComponents(query) {
        const items = document.querySelectorAll('.component-item');
        const lowerQuery = query.toLowerCase();
        
        items.forEach(item => {
            const text = item.textContent.toLowerCase();
            item.style.display = text.includes(lowerQuery) ? 'flex' : 'none';
        });
    }
    
    escapeHtml(text) {
        const div = document.createElement('div');
        div.textContent = text;
        return div.innerHTML;
    }
}

// Initialize runtime when DOM is ready
document.addEventListener('DOMContentLoaded', () => {
    window.droyRuntime = new DroyRuntime();
});
