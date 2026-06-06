import React, {useState, useRef, useEffect} from 'react'
import Editor from '@monaco-editor/react'
import axios from 'axios'

const SAMPLES = {
  hello: { name: 'Hello', file: 'hello.mj' },
  factorial: { name: 'Factorial', file: 'factorial.mj' },
  arrays: { name: 'Arrays & Chars', file: 'arrays_and_chars.mj' },
  invalid_semicolon: { name: 'Invalid (missing ;)', file: 'invalid_missing_semicolon.mj' },
  invalid_semantic: { name: 'Invalid (semantic)', file: 'invalid_semantic.mj' },
}

const MODES = [
  { label: 'All Parsers', value: 'all' },
  { label: 'Lexer', value: 'lexer' },
  { label: 'Recursive Descent', value: 'recursive' },
  { label: 'Predictive', value: 'predictive' },
  { label: 'LR Parser', value: 'lr' },
]

export default function App(){
  const [code, setCode] = useState(`class Hello {\n  void main() {\n    print(1 + 2 * 3);\n    return;\n  }\n}`)
  const [mode, setMode] = useState('all')
  const [output, setOutput] = useState('')
  const [busy, setBusy] = useState(false)
  const [outputTab, setOutputTab] = useState('errors')
  const [stderr, setStderr] = useState('')
  const [returnCode, setReturnCode] = useState(null)
  const editorRef = useRef(null)
  const monacoRef = useRef(null)
  const decorationIds = useRef([])
  const [errors, setErrors] = useState([])

  useEffect(() => {
    const handleKeyDown = (e) => {
      if ((e.ctrlKey || e.metaKey) && e.key === 'Enter') {
        run()
      }
    }
    window.addEventListener('keydown', handleKeyDown)
    return () => window.removeEventListener('keydown', handleKeyDown)
  }, [code, mode])

  const loadSample = async (key) => {
    try {
      const res = await fetch(`/test/samples/${SAMPLES[key].file}`)
      const text = await res.text()
      setCode(text)
      setErrors([])
      setOutput('')
      setStderr('')
      setOutputTab('errors')
      decorationIds.current = editorRef.current?.deltaDecorations(decorationIds.current, []) || []
    } catch(e) {
      setOutput(`Failed to load sample: ${e.message}`)
    }
  }

  const downloadCode = () => {
    const el = document.createElement('a')
    el.href = URL.createObjectURL(new Blob([code], {type: 'text/plain'}))
    el.download = 'code.mj'
    el.click()
  }

  const run = async () => {
    setBusy(true)
    setOutput('Compiling...')
    try{
      const res = await axios.post('http://127.0.0.1:5000/api/compile', { source: code, mode })
      setOutput(res.data.stdout || '')
      setStderr(res.data.stderr || '')
      setReturnCode(res.data.returncode)
      setOutputTab('errors')

      const errs = res.data.errors || []
      setErrors(errs)

      if (editorRef.current && monacoRef.current) {
        const monaco = monacoRef.current
        const editor = editorRef.current
        const newDecorations = errs.map(e => ({
          range: new monaco.Range(e.line, e.column, e.line, Math.max(1, e.column + 1)),
          options: {
            inlineClassName: 'mjErrorInline',
            hoverMessage: { value: `${e.kind} error: ${e.message}` }
          }
        }))
        decorationIds.current = editor.deltaDecorations(decorationIds.current, newDecorations)
      }
    }catch(e){
      setOutput('')
      setStderr(String(e))
      setErrors([])
      setOutputTab('stderr')
    }finally{
      setBusy(false)
    }
  }

  function handleEditorMount(editor, monaco) {
    editorRef.current = editor
    monacoRef.current = monaco
  }

  function goToError(e){
    if (!editorRef.current) return
    editorRef.current.revealPositionInCenter({lineNumber: e.line, column: e.column})
    editorRef.current.setPosition({lineNumber: e.line, column: e.column})
    editorRef.current.focus()
  }

  const outTabContent = outputTab === 'errors'
    ? (
      <div>
        <div style={{color: '#666', fontSize:12, marginBottom:8}}>
          {errors.length === 0 ? (
            <div style={{color:'#28a745'}}>✓ No errors</div>
          ) : (
            <div style={{color:'#dc3545'}}>{errors.length} error(s) found</div>
          )}
        </div>
        <ul style={{paddingLeft:16, margin:0, fontSize:13}}>
          {errors.map((e, i)=> (
            <li key={i} style={{cursor:'pointer', marginBottom:6, color:'#0066cc'}} onClick={()=>goToError(e)}>
              <strong>{e.kind}</strong> {e.line}:{e.column} — {e.message}
            </li>
          ))}
        </ul>
      </div>
    )
    : outputTab === 'stdout'
      ? (<pre style={{margin:0, fontSize:12, whiteSpace:'pre-wrap', wordBreak:'break-word'}}>{output || '(empty)'}</pre>)
      : (<pre style={{margin:0, fontSize:12, whiteSpace:'pre-wrap', wordBreak:'break-word', color:'#c33'}}>{stderr || '(empty)'}</pre>)

  return (
    <div style={{height:'100vh',display:'flex',flexDirection:'column', fontFamily:'Segoe UI,Tahoma,Geneva,Verdana,sans-serif'}}>
      {/* Header */}
      <div style={{background:'#f8f9fa', borderBottom:'1px solid #dee2e6', padding:'12px 16px'}}>
        <div style={{display:'flex', justifyContent:'space-between', alignItems:'center'}}>
          <div>
            <h2 style={{margin:'0 0 8px 0', fontSize:18}}>MicroJava Web IDE</h2>
            <div style={{fontSize:12, color:'#666'}}>Compile and analyze MicroJava source code</div>
          </div>
          <div style={{display:'flex', gap:8}}>
            <select
              value={mode}
              onChange={(e)=>setMode(e.target.value)}
              style={{padding:'6px 10px', borderRadius:4, border:'1px solid #ccc', fontSize:13}}
              disabled={busy}
            >
              {MODES.map(m => <option key={m.value} value={m.value}>{m.label}</option>)}
            </select>
            <button onClick={run} disabled={busy} style={{padding:'6px 16px', background:'#007bff', color:'white', border:'none', borderRadius:4, cursor:'pointer', fontSize:13, fontWeight:500}}>
              {busy ? 'Compiling...' : 'Compile (Ctrl+Enter)'}
            </button>
          </div>
        </div>
      </div>

      {/* Main Layout */}
      <div style={{flex:'1 1 0',display:'flex', overflow:'hidden'}}>
        {/* Editor */}
        <div style={{flex:'1 1 60%', display:'flex', flexDirection:'column', borderRight:'1px solid #dee2e6'}}>
          <div style={{background:'#f1f3f5', borderBottom:'1px solid #dee2e6', padding:'8px 12px', display:'flex', gap:8, flexWrap:'wrap', overflowY:'auto', maxHeight:60}}>
            <div style={{fontSize:12, color:'#666', marginRight:8}}>Samples:</div>
            {Object.entries(SAMPLES).map(([key, {name}]) => (
              <button
                key={key}
                onClick={() => loadSample(key)}
                disabled={busy}
                style={{padding:'4px 10px', background:'white', border:'1px solid #ccc', borderRadius:3, cursor:'pointer', fontSize:12}}
              >
                {name}
              </button>
            ))}
            <button
              onClick={downloadCode}
              style={{padding:'4px 10px', background:'#28a745', color:'white', border:'none', borderRadius:3, cursor:'pointer', fontSize:12, marginLeft:'auto'}}
            >
              Download
            </button>
          </div>
          <div style={{flex:1}}>
            <Editor
              height="100%"
              defaultLanguage="java"
              value={code}
              onChange={(v)=>setCode(v||'')}
              onMount={handleEditorMount}
              options={{fontSize:13, lineNumbers:'on', automaticLayout:true}}
            />
          </div>
        </div>

        {/* Output Panel */}
        <div style={{flex:'1 1 40%', display:'flex', flexDirection:'column', background:'#fafbfc'}}>
          <div style={{padding:'8px 12px', borderBottom:'1px solid #dee2e6', display:'flex', gap:8, background:'#f1f3f5'}}>
            {['errors', 'stdout', 'stderr'].map(tab => (
              <button
                key={tab}
                onClick={() => setOutputTab(tab)}
                style={{
                  padding:'6px 12px',
                  background: outputTab === tab ? '#007bff' : 'white',
                  color: outputTab === tab ? 'white' : '#333',
                  border: '1px solid ' + (outputTab === tab ? '#007bff' : '#ccc'),
                  borderRadius:'3px 3px 0 0',
                  cursor:'pointer',
                  fontSize:12,
                  fontWeight: outputTab === tab ? 'bold' : 'normal'
                }}
              >
                {tab.toUpperCase()}
              </button>
            ))}
            {returnCode !== null && (
              <div style={{marginLeft:'auto', fontSize:12, color:'#666', paddingTop:6}}>
                Exit: <strong>{returnCode}</strong>
              </div>
            )}
          </div>
          <div style={{flex:1, padding:'12px', overflow:'auto', fontSize:12, lineHeight:1.5}}>
            {outTabContent}
          </div>
        </div>
      </div>

      {/* Footer */}
      <div style={{background:'#f8f9fa', borderTop:'1px solid #dee2e6', padding:'8px 16px', fontSize:11, color:'#666'}}>
        Tip: Use Ctrl+Enter to compile | Lexer (scanning) → Recursive/Predictive (LL parsing) → LR (SLR parsing)
      </div>
    </div>
  )
}

